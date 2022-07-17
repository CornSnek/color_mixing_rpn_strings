#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <span>
#include <memory>
#include <SFML/Graphics.hpp>
#include "color_mixing.hpp"
#include "generators.hpp"
#include "rpn_container.hpp"
#include "game_input.hpp"
#include "camera_impl.hpp"
#include "fps_count.hpp"
#include "event_dispatcher.hpp"
#include "gui_things.hpp"
#include "shared_constants.hpp"
using RPN_CStr=std::unique_ptr<const char>;//To delete const char* allocation by itself.
char char_set[]={'B','R','Y'};//Blue, Red, Yellow
BRYVector char_color[]={BRYVector('B'),BRYVector('R'),BRYVector('Y')};
sf::Vector2f char_vector[]={sf::Vector2f(-TriLen/2.0f,LongTriLen()/2.0f),sf::Vector2f(0.0f,-LongTriLen()/2.0f),sf::Vector2f(TriLen/2.0f,LongTriLen()/2.0f)};
const auto span_char_set=std::span<char>(char_set,sizeof(char_set));
const auto char_color_span=std::span<BRYVector>(char_color,sizeof(char_color));
const auto char_vector_span=std::span<sf::Vector2f>(char_vector,sizeof(char_vector));
RPNContainer rpn_c;
void get_rpns(bool& done,std::mutex& c_str_mutex,std::size_t&rpn_progress,std::size_t&rpn_max,std::size_t&char_length){
	for(std::size_t i=0;i!=sizeof(char_set);i++){
		rpn_c.add_char(char_color[i],char_set[i]);//Add init characters.
	}
	std::cout<<rpn_c.total_colors()<<" RPN string colors found. "<<rpn_c.total_inserted()<<" RPN strings inserted."<<std::endl;
	for(std::size_t CharLength=2;CharLength<=MaxCharLength;CharLength++){
		int add_spr_counter=0;
		MultiSetStringGen mssg(CharLength,span_char_set);
		DyckWordsGen dwg(CharLength-1);
		dwg.verify_multiset_rpn(mssg);
		const std::size_t DWG_Cat=dwg.get_max_permutations();
		const std::size_t MSSG_Perm=mssg.get_max_permutations();
		c_str_mutex.lock();
		rpn_max=MSSG_Perm;
		char_length=CharLength;
		c_str_mutex.unlock();
		auto start=std::chrono::steady_clock::now();
		for(std::size_t ms_sg_i=0;ms_sg_i!=MSSG_Perm;ms_sg_i++){
			if(done) return;
			for(std::size_t dwg_i=0;dwg_i!=DWG_Cat;dwg_i++){
				std::span<char>span_c(dwg.next_rpn(mssg));
				auto[is_unmergeable,cmr]=verify_unmerged_rpn(span_c);
				if(is_unmergeable){
					rpn_c.add_if_unique(rpn_to_color(span_c,span_char_set,char_color_span),cmr);
				}
				delete[] span_c.data();
			}
			c_str_mutex.lock();
			rpn_progress=ms_sg_i+1;//+1 Because counting from 1 to MSSG_Perm and not 0 to MSSG_Perm-1
			c_str_mutex.unlock();
			mssg.next();
			if(add_spr_counter++==500){
				rpn_c.copy_map_to_parse();
				add_spr_counter=0;
			}
		}
		auto end=std::chrono::steady_clock::now();
		std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<" ms counted for CharLength = "<<std::to_string(CharLength)<<", total of "<<rpn_c.total_colors()<<" RPN string colors found. "<<rpn_c.total_inserted()<<" RPN strings inserted."<<std::endl;
	}
	rpn_c.copy_map_to_parse();
	c_str_mutex.lock();
	done=true;
	c_str_mutex.unlock();
}


namespace ForMinimap{
	const sf::Color sf_c_gray=sf::Color(0x1f1f1f7f);
	const sf::Color sf_c_blue=sf::Color(0xff00007f);
	const sf::Color sf_c_yellow=sf::Color(0xffff007f);
	const sf::Color sf_c_red=sf::Color(0x0000ff7f);
	const sf::Color sf_c_green=sf::Color(0x00ff007f);
	const sf::Color sf_c_magenta=sf::Color(0xff00ff7f);
	const sf::Vertex minimap_vb[]={
		sf::Vertex(char_vector[0],sf_c_red),
		sf::Vertex(char_vector[2],sf_c_yellow),
		sf::Vertex(char_vector[1],sf_c_blue),
		sf::Vertex(sf::Vector2f(0.f,LongTriLen()/2.f),sf_c_green),
		sf::Vertex(sf::Vector2f(0.f,TriLen*sqrt(3)/12),sf_c_gray),
		sf::Vertex(sf::Vector2f(-TriLen/4,0),sf_c_magenta)
	};
	const sf::Vertex l_tri_left[]={minimap_vb[0],minimap_vb[3],minimap_vb[4]};
	const sf::Vertex l_tri_right[]={minimap_vb[1],minimap_vb[3],minimap_vb[4]};
	const sf::Vertex u_tri_right[]={minimap_vb[1],minimap_vb[2],minimap_vb[4]};
	const sf::Vertex u_tri_left_lower[]={minimap_vb[0],minimap_vb[4],minimap_vb[5]};
	const sf::Vertex u_tri_left_upper[]={minimap_vb[2],minimap_vb[4],minimap_vb[5]};
	const sf::Vertex minimap_rect[]={
		sf::Vertex(sf::Vector2f(-TriLen*0.6f,-TriLen*0.5f),sf::Color::White),
		sf::Vertex(sf::Vector2f(TriLen*0.6f,-TriLen*0.5f),sf::Color::White),
		sf::Vertex(sf::Vector2f(TriLen*0.6f,TriLen*0.5f),sf::Color::White),
		sf::Vertex(sf::Vector2f(-TriLen*0.6f,TriLen*0.5f),sf::Color::White),
		sf::Vertex(sf::Vector2f(-TriLen*0.6f,-TriLen*0.5f),sf::Color::White)
	};
}//-TriLen*0.6f-10,-TriLen*0.5f-10,TriLen*1.2f+20,TriLen+20
Camera camera_obj(ScreenWidth,ScreenHeight);
int main(/*int argc,char* argv[]*/)
{
	sf::Font* font=new sf::Font();//Shared font.
	font->loadFromFile("../assets/Kenney Pixel.ttf");
	camera_obj.set_camera_bounds(
		new CameraBounds(camera_obj
		,sf::FloatRect(-TriLen*0.6f,-TriLen*0.5f,TriLen*0.6f,TriLen*0.5f)
		,0.01f,10.0f));
	ProgressBar pb(sf::FloatRect(0.0f,ScreenHeight-35.0f,ScreenWidth,35.f),font);
	AboutSection ab_s(AboutSectionStr,font,sf::FloatRect(100,100,ScreenWidth-200,ScreenHeight-200));
	GUIButton about_show_button("About\nProgram\n(Show\n/Hide)",font,sf::FloatRect(ScreenWidth-100,0,100,100));
	about_show_button.new_callback([](AnyData ab_sv){
		auto ab_sp=static_cast<AboutSection*>(ab_sv);
        ab_sp->show_gui=!ab_sp->show_gui;//Toggle AboutSection GUI.
	},&ab_s);
	Minimap mm;
	std::size_t rpn_progress=0,rpn_max=0,char_length=1;
	RPNColorSprites rpn_cs;
	bool done=false; //For rpn_get_sprites to end their process if rpn_routine is done.
	std::mutex c_str_mutex; //For reading c_str before it gets deleted/replaced.
	std::thread rpn_routine(get_rpns,std::ref(done),std::ref(c_str_mutex),std::ref(rpn_progress),std::ref(rpn_max),std::ref(char_length));
	std::thread rpn_get_sprites(&RPNContainer::add_color_sprites
		,std::ref(rpn_c),std::ref(done),std::ref(rpn_cs),span_char_set,char_vector_span,std::ref(font));
	sf::RenderWindow r_window(sf::VideoMode(ScreenWidth, ScreenHeight), "Color Mixing RPN Strings",
	sf::Style::Default^sf::Style::Resize);//No resize.
	r_window.setPosition(sf::Vector2i(0,0));
	{
		{
			using namespace ForMinimap;
			mm.add(l_tri_left,3,sf::Triangles);
			mm.add(l_tri_right,3,sf::Triangles);
			mm.add(u_tri_right,3,sf::Triangles);
			mm.add(u_tri_left_lower,3,sf::Triangles);
			mm.add(u_tri_left_upper,3,sf::Triangles);
			mm.add(minimap_rect,5,sf::LineStrip);
		}
		sf::Text debug_text;
		bool debug_text_show=false;
		GUIButton debug_text_button("Show/Hide\nDebug Text",font,sf::FloatRect(ScreenWidth-200,0,100,100));
		debug_text_button.new_callback([](AnyData debug_text_show_v){
			auto debug_text_show_p=static_cast<bool*>(debug_text_show_v);
			*debug_text_show_p=!*debug_text_show_p;
		},&debug_text_show);
		debug_text.setFont(*font);
		debug_text.setCharacterSize(32);
		debug_text.setFillColor(sf::Color::White);
		debug_text.setOutlineThickness(2.f);
		debug_text.setOutlineColor(sf::Color::Black);
		debug_text.setStyle(sf::Text::Bold);
		sf::Clock clock;
		FPSCount fps_count;
		for(auto [frame_count,now,prev,lag,event,game_input]
		=std::tuple(std::size_t(),clock.getElapsedTime().asMilliseconds(),sf::Int32(),sf::Int32(),sf::Event(),GameInput());;){
			prev=now;
			now=clock.getElapsedTime().asMilliseconds();
			if(now-prev<FPMS){//Sleep until next ms frame if too fast...
				using namespace std::chrono_literals;
				std::this_thread::sleep_for((FPMS-(now-prev))*1ms);
				now=clock.getElapsedTime().asMilliseconds();
			}
			fps_count.update();
			lag+=now-prev;
			while(lag>=FPMS){//Fixed frame rate Game Loop.
				//Update inputs...
				while (r_window.pollEvent(event)){
					switch(event.type){
					case(sf::Event::Closed):
						goto sfml_done;
					case(sf::Event::KeyPressed):
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
							goto sfml_done;
						}
						game_input.set_key_down(event.key.code);
						break;
					case(sf::Event::KeyReleased):
						game_input.set_key_up(event.key.code);
						break;
					case(sf::Event::MouseMoved): [[fallthrough]];
					case(sf::Event::MouseButtonPressed): [[fallthrough]];
					case(sf::Event::MouseButtonReleased):
						camera_obj.mouse_to_world(event);
						debug_text_button.update(event);
						about_show_button.update(event);
						break;
					default: break;
					}
				}
				//Update game logics...
				camera_obj.input_update(game_input);
				ColorSprite::sprite_update(camera_obj);
				lag-=FPMS;
				frame_count++;
			}
			//Draw...
			r_window.clear(sf::Color(0x3f3f3fff));
			camera_obj.set_to_world_view(r_window);
			std::size_t sprites_drawn=0;
			for(auto& p:rpn_cs){
				if(p.second.draw_to_window(r_window,camera_obj,rpn_c)) sprites_drawn++;
			}
			camera_obj.set_to_gui_view(r_window);
			pb.draw(r_window);
			c_str_mutex.lock();
			pb.update(rpn_progress,rpn_max,char_length);
			debug_text.setString(std::to_string(fps_count.get_fps())+" FPS\nSprites Drawn: "+std::to_string(sprites_drawn)+"\n"+std::to_string(rpn_c.total_inserted())+" Color RPN strings inserted. \n"+std::to_string(rpn_c.total_colors())+" Total unique colors.\n"+camera_obj.get_camera_info());
			c_str_mutex.unlock();
			about_show_button.draw(r_window);
			debug_text_button.draw(r_window);
			r_window.setView(camera_obj.get_minimap_view(sf::FloatRect(0,0,300,300),sf::FloatRect(-TriLen*0.6f-10,-TriLen*0.5f-10,TriLen*1.2f+20,TriLen+20)));
			mm.draw(r_window,camera_obj);
			camera_obj.set_to_gui_view(r_window);
			ab_s.draw(r_window);
			if(debug_text_show) r_window.draw(debug_text);
			r_window.display();
		}
	}
	sfml_done:
	delete font;
	r_window.close();//Destroy the sf::objects first then close to prevent weird memory errors from textures.
	c_str_mutex.lock();
	done=true;
	c_str_mutex.unlock();
	if(rpn_routine.joinable()) rpn_routine.join();
	if(rpn_get_sprites.joinable()) rpn_get_sprites.join();
	std::ofstream output_file;
	output_file.open("output.txt");
	rpn_c.write_to_file(output_file);
	output_file.close();
	return 0;
}
