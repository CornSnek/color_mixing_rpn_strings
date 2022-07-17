#include "gui_things.hpp"
ProgressBar::ProgressBar(sf::FloatRect position,sf::Font* font):progress(0),total(1)
    ,position(sf::Vector2f(position.left,position.top)),size(sf::Vector2f(position.width,position.height))
    ,bar_container(),bar(),text(){
    bar.setPosition(position.left,position.top);
    bar.setSize(size);
    bar_container=bar;
    bar_container.setFillColor(sf::Color(0x0000003f));
    bar.setFillColor(sf::Color(0xffffff9f));
    text.setFont(*font);
	text.setCharacterSize(32);
	text.setFillColor(sf::Color::White);
	text.setOutlineThickness(2.f);
	text.setOutlineColor(sf::Color::Black);
	text.setStyle(sf::Text::Bold);
}
void ProgressBar::update(std::size_t progress,std::size_t total,std::size_t char_length){
    this->progress=progress;
    this->total=total;
    text.setString(std::string("Parsing RPN Strings (")+MixNamesCapitals[char_length]+"): "+std::to_string(progress)+"/"+std::to_string(total));
}
#define __FRAC(n,d) (static_cast<float>(n)/static_cast<float>(d))
void ProgressBar::draw(sf::RenderWindow& rw){
    rw.draw(bar_container);
    if(total){//No divide by 0.
        bar.setSize(sf::Vector2f(size.x*__FRAC(progress,total),size.y));
    }
    text.setPosition(position.x+size.x/2,position.y+size.y/2-16);
    const sf::FloatRect text_box_size=text.getLocalBounds();
    text.move(-text_box_size.width/2,-text_box_size.height/2); //Center text.
    rw.draw(bar);
    rw.draw(text);
}
AboutSection::AboutSection(const char* str,sf::Font* font,sf::FloatRect position):rect(),text(),show_gui(false){
    text.setFont(*font);
	text.setCharacterSize(32);
	text.setFillColor(sf::Color::White);
	text.setOutlineThickness(2.f);
	text.setOutlineColor(sf::Color::Black);
	text.setStyle(sf::Text::Bold);
    text.setString(str);
    text.setPosition(position.left+10,position.top);
    rect.setPosition(position.left,position.top);
    rect.setSize(sf::Vector2f(position.width,position.height));
    rect.setFillColor(sf::Color(0x000000bf));
}
void AboutSection::draw(sf::RenderWindow& rw){
    if(show_gui){
        rw.draw(rect);
        rw.draw(text);
    }
}
GUIButton::GUIButton(const char* str,sf::Font* font,sf::FloatRect position):rect(),text(),callback(NULLCallback),any_data(nullptr){
    text.setFont(*font);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setOutlineThickness(2.f);
	text.setOutlineColor(sf::Color::Black);
	text.setStyle(sf::Text::Bold);
    text.setString(str);
    text.setPosition(position.left+10,position.top);
    rect.setPosition(position.left,position.top);
    rect.setSize(sf::Vector2f(position.width,position.height));
    rect.setFillColor(sf::Color(0xffffff7f));
}
void GUIButton::update(sf::Event& mouse_event){
    switch(mouse_event.type){
        case(sf::Event::MouseMoved):
            if(rect.getGlobalBounds().contains(mouse_event.mouseMove.x,mouse_event.mouseMove.y))
                rect.setFillColor(sf::Color::White);
            else rect.setFillColor(sf::Color(0xffffff7f));
            break;
        case(sf::Event::MouseButtonPressed):
            if(rect.getGlobalBounds().contains(mouse_event.mouseButton.x,mouse_event.mouseButton.y))
                callback(any_data);
            break;
        case(sf::Event::MouseButtonReleased): break;
        default: throw std::logic_error("Event not implemented for GUIButton::update");
    }
}
void GUIButton::new_callback(ButtonCallback bcb,AnyData any_d){
    callback=bcb;
    any_data=any_d;
}
void GUIButton::draw(sf::RenderWindow& rw){
    rw.draw(rect);
    rw.draw(text);
}
Minimap::Minimap():vbuffer(),properties(){}
void Minimap::add(const sf::Vertex* buffer,std::size_t size,sf::PrimitiveType type){
	std::size_t begin_i=vbuffer.size();
	for(std::size_t i=0;i<size;i++) vbuffer.push_back(buffer[i]);
	properties.push_back(std::make_tuple(begin_i,size,type));
}
void Minimap::draw(sf::RenderWindow& r_window,Camera& camera){
	for(auto p:properties){
		r_window.draw(&vbuffer[std::get<0>(p)],std::get<1>(p),std::get<2>(p));
	}
	sf::Vector2f center;
	float zoom_scale,rotation;
	camera.get_camera_properties(center,zoom_scale,rotation);
	sf::Vector2f camera_v_buffer[]={
		sf::Vector2f(
			(std::cos(ToRad(rotation))*ScreenWidth/2-std::sin(ToRad(rotation))*ScreenHeight/2)
			,(std::sin(ToRad(rotation))*ScreenWidth/2+std::cos(ToRad(rotation))*ScreenHeight/2)),
		sf::Vector2f(
			(-std::cos(ToRad(rotation))*ScreenWidth/2-std::sin(ToRad(rotation))*ScreenHeight/2)
			,(-std::sin(ToRad(rotation))*ScreenWidth/2+std::cos(ToRad(rotation))*ScreenHeight/2)),
		sf::Vector2f(
			(-std::cos(ToRad(rotation))*ScreenWidth/2+std::sin(ToRad(rotation))*ScreenHeight/2)
			,(-std::sin(ToRad(rotation))*ScreenWidth/2-std::cos(ToRad(rotation))*ScreenHeight/2)),
		sf::Vector2f(
			(std::cos(ToRad(rotation))*ScreenWidth/2+std::sin(ToRad(rotation))*ScreenHeight/2)
			,(std::sin(ToRad(rotation))*ScreenWidth/2-std::cos(ToRad(rotation))*ScreenHeight/2))
	};
	sf::Vertex camera_v[]={
		sf::Vertex(center+camera_v_buffer[0]*zoom_scale,sf::Color::Yellow),
		sf::Vertex(center+camera_v_buffer[1]*zoom_scale,sf::Color::Blue),
		sf::Vertex(center+camera_v_buffer[2]*zoom_scale,sf::Color::Magenta),
		sf::Vertex(center+camera_v_buffer[3]*zoom_scale,sf::Color::Red),
		sf::Vertex(center+camera_v_buffer[0]*zoom_scale,sf::Color::Yellow)
	};
	r_window.draw(camera_v,5,sf::LineStrip);
}