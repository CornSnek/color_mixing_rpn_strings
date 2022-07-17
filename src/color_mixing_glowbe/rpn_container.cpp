#include "rpn_container.hpp"

std::mutex RPNContainer::rw_mutex=std::mutex();
std::mutex RPNContainer::parse_mutex=std::mutex();
std::condition_variable RPNContainer::parse_hv=std::condition_variable();
RPNContainer::RPNContainer():uom(),uom_to_parse(),_total_inserted(0){}
RPNContainer::~RPNContainer(){
    for(auto& p:uom) for(auto& c_str:p.second) delete[] c_str;
}
/*Debugging purposes to check if there's no memory leak using destructor.*/
RPNContainer::RPNContainer(RPNContainer&& to_move):uom(std::move(to_move.uom)),uom_to_parse(),_total_inserted(0){}

bool RPNContainer::add_if_unique(BRYVector bry_v,ColorMidpointRPN& cmr){
    std::lock_guard<std::mutex> rw(rw_mutex);
    const char* add_this=cmr.copy_to_c_str();
    auto old_p=(this->uom.find(bry_v));
    if(old_p!=uom.end()){
        if(strlen(add_this)>strlen(old_p->second.at(0))){//Don't add any longer strings.
            delete[] add_this;
            return false;
        }
        for(auto& c_str:old_p->second){
            if(!strcmp(c_str,add_this)){//Don't add duplicates (==0).
                delete[] add_this;
                return false;
            }
        }
        old_p->second.push_back(add_this);
    }else{
        RPNVector v;
        v.push_back(add_this);
        uom.insert(std::make_pair(bry_v,v));
    }
    _total_inserted++;
    return true;
}
void RPNContainer::add_char(BRYVector bry_v,const char c){
    std::lock_guard<std::mutex> rw(rw_mutex);
    char* as_null_term=new char[2]();
    as_null_term[0]=c; as_null_term[1]='\0';
    if(uom.find(bry_v)==uom.end()){
        RPNVector v;
        v.push_back(as_null_term);
        uom.insert(std::make_pair(bry_v,v));
        _total_inserted++;
    }else throw std::logic_error("No duplicates.");
}
void RPNContainer::copy_map_to_parse(){
    std::lock_guard<std::mutex> parse(parse_mutex); //Wait until add_color_sprites is done.
    uom_to_parse=uom;
    parse_hv.notify_one();
}
void RPNContainer::add_color_sprites(bool& done,RPNColorSprites& rpn_cs,const std::span<char>& char_span,const std::span<sf::Vector2f>& char_vector_span, sf::Font*&shared_font){
    while(true){
        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> parse_ul(parse_mutex);
        while(uom_to_parse.empty()){if(done){return;} parse_hv.wait_for(parse_ul,2s);} //Prevent spurious wakeup via loop uom_to_parse.empty(), and end if done.
        for(const auto& p:uom_to_parse){
            if(rpn_cs.find(p.first)==rpn_cs.end()){//Unique BRYVectors only.
                const char* c_str=p.second.at(0);//Just compare the first vector (should be the shortest).
                std::size_t c_str_len=strlen(c_str);
                RPNStack<sf::Vector2f> v_stack(c_str_len);
                std::size_t char_i=-1,rpn_i=0;
                float mouse_zoom_mult=1.0f;
	            for(;rpn_i!=c_str_len;rpn_i++){
                    char_i=-1;
                    char ch=c_str[rpn_i];
                    if(ch!='+'){//Non '+', put index color# Vector2f by char in stack.
                        while(char_span[++char_i]!=ch){}
                        v_stack.push(char_vector_span[char_i]);
                        continue;
                    }//For '+' operator, "Merge last two elements from stack by midpoints"
                    v_stack.push((v_stack.pop()+v_stack.pop())/2.0f);
                    mouse_zoom_mult/=std::sqrt(2.0f);
                }
                sf::Vector2f midpoint=v_stack.pop();
                rpn_cs.insert(std::make_pair(p.first,ColorSprite(mouse_zoom_mult,midpoint,p.first,shared_font)));
            }
        }
        uom_to_parse.clear();
    }
}
/*Since RPNVector may change address, get pointer by BRYVector, or nullptr if it doesn't exist.*/
/*Note: Not thread safe. Needs to be locked with RPNContainer::rw_mutex.*/
RPNVector* RPNContainer::get_rpnvector_by(BRYVector bry_v){
    auto it=uom.find(bry_v);
    RPNVector* return_this=nullptr;
    if(it!=uom.end()){
        return_this=&(it->second);
    }
    return return_this;
}
std::size_t RPNContainer::total_colors(){
    return uom.size();
}
std::size_t RPNContainer::total_inserted(){
    return _total_inserted;
}
/*For debugging purposes*/
void RPNContainer::write_to_file(std::ofstream& output){
    for(const auto& p:uom){
        output<<std::hex<<std::setfill('0')<<std::setw(16);
        output<<p.first.to_hash();
        for(const auto& c_str:p.second){
            output<<"|"<<c_str;
        }
        output<<"\n";
        //output<<std::resetiosflags(std::ios::basefield);
    }
}
sf::Text* ColorSprite::last_with_text=nullptr; //static
float ColorSprite::rotation=0;//static
const float ColorSprite::triangle_side=7680.f/std::pow(2,MaxCharLength);//static
ColorSprite::ColorSprite(float min_draw_zoom,sf::Vector2f world_position,BRYVector bry_v,sf::Font* font)
    :show_rpns(false),min_draw_zoom(min_draw_zoom),triangle(3),world_position(world_position),bry_v(bry_v),text(){
        const float longest=triangle_side*std::sqrt(3.0f)/2.0f;
        triangle.setPoint(0,sf::Vector2f(triangle_side/2.0f,0.0f));
        triangle.setPoint(1,sf::Vector2f(0.0f,longest));
        triangle.setPoint(2,sf::Vector2f(triangle_side,longest));//Todo equilateral triangle.
        triangle.setFillColor(sf::Color::Black);
        triangle.setOutlineThickness(0.0f);
        triangle.setOutlineColor(sf::Color::Black);
        triangle.setPosition(world_position);
        triangle.setOrigin(triangle_side/2.0f,triangle_side/std::sqrt(3.0f));//Center it.
        text.setPosition(world_position);
        text.setFont(*font);
        text.setCharacterSize(24);
		text.setFillColor(sf::Color::White);
		text.setOutlineThickness(2.f);
		text.setOutlineColor(sf::Color::Black);
		text.setStyle(sf::Text::Bold);
}
ColorSprite::~ColorSprite(){}
//static
void ColorSprite::sprite_update(Camera& /*cam*/){
    rotation=fmodf(rotation+2.0f,360.0f);
}
float distance_squared(float x1,float y1,float x2,float y2){
    return powf(x2-x1,2.f)+powf(y2-y1,2.f);
}
void ColorSprite::call(const EventData& ud){
    float ds=distance_squared(ud.data.world_coords.x,ud.data.world_coords.y,world_position.x,world_position.y);
    if(ds<=powf(triangle_side*ud.data.world_coords.zoom,2.f)){
        const sf::Event mouse_event=ud.data.world_coords.mouse_event;
        if(mouse_event.type==sf::Event::MouseButtonPressed&&mouse_event.mouseButton.button==sf::Mouse::Left){
            show_rpns=true;
        }
    }
}
bool ColorSprite::draw_to_window(sf::RenderWindow& rw,Camera& cam,RPNContainer& rpn_c){
    float cam_zoom_scale=cam.get_zoom_scale();
    float cam_max_zoom=cam.get_camera_bounds().max_zoom;
    text.setScale(cam.get_zoom_scale(),cam.get_zoom_scale());
    if(cam.world_view_can_draw(triangle.getGlobalBounds())){
        RPNContainer::rw_mutex.lock();
        auto rpn_v=rpn_c.get_rpnvector_by(bry_v);
        const std::size_t v_len=rpn_v->size();
        const BRYColorWheel bry_colorw(bry_v);
        const unsigned int bry_color_trans=bry_colorw.get_color(0x00);
        if(show_rpns){
            std::string text_concat;
            std::string B_s,R_s,Y_s;
            text_concat+=std::to_string(v_len)+" string"+(v_len!=1?"s":"");
            bry_v.get_fractions(B_s,R_s,Y_s);
            text_concat+="\n"+B_s+", "+R_s+", "+Y_s+"\n";
            text_concat+=MixNamesCapitals[strlen(rpn_v->at(0))/2+1];
            for(std::size_t i=0;i<rpn_v->size();i++){
                ColorMidpointInfix cmi=rpn_to_infix(rpn_v->at(i));
                const char* cmi_s=cmi.get_value();
                text_concat+=std::string("\n")+rpn_v->at(i)+" or "+cmi_s;
                delete cmi_s;
            }
            if(last_with_text) last_with_text->setString("");//Remove last ColorSprite with text.
            text.setString(text_concat);
            last_with_text=&text;
            show_rpns=false;
        }
        if(last_with_text!=&text) text.setString(std::to_string(v_len)+" "+MixNamesCapitals[strlen(rpn_v->at(0))/2+1]);
        RPNContainer::rw_mutex.unlock();
        triangle.setOutlineThickness(((v_len-1)?1.0:0.0f)*powf(1.13f,v_len));
        text.setFillColor(sf::Color(bry_color_trans|0xff));
        const bool color_bright=((bry_color_trans&0xff000000u)>>24)*0.299f
            +((bry_color_trans&0x00ff0000u)>>16)*0.587f
            +((bry_color_trans&0x0000ff00u)>>8)*0.114f>150.f;
        if(color_bright) text.setOutlineColor(sf::Color::Black);
        else text.setOutlineColor(sf::Color::White);
        //if (red*0.299 + green*0.587 + blue*0.114) > 150 use #000000 else use #ffffff
        if(cam_zoom_scale<=cam_max_zoom*min_draw_zoom){//To highlight and rotate triangles.
            triangle.setFillColor(sf::Color(bry_color_trans|0xff));
            triangle.setOutlineColor(sf::Color(bry_color_trans|0x7f));
            triangle.setScale(cam_zoom_scale,cam_zoom_scale);
            triangle.setRotation(rotation);
            rw.draw(triangle);
            rw.draw(text);//Draw text after triangle.
            EventFor<ColorSprite>::register_obj(this);//Register for mouse listening.
        }else{
            triangle.setFillColor(sf::Color(bry_color_trans|0xff));
            triangle.setOutlineColor(sf::Color(bry_color_trans|0x1f));
            triangle.setScale(1.0f,1.0f);
            triangle.setRotation(0.0f);
            rw.draw(triangle);
            EventFor<ColorSprite>::deregister_obj(this->get_obj_id());//Deregister if not within zoom limits.
        }
        return true;
    }
    if(cam.world_view_can_draw(triangle.getGlobalBounds())) rw.draw(text);//Don't draw if triangle isn't there.
    EventFor<ColorSprite>::deregister_obj(this->get_obj_id());//Deregister if out of bounds of window.
    return false;
}