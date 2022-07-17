#ifndef _PROGRESS_BAR_HPP_
#define _PROGRESS_BAR_HPP_
#include <string>
#include <SFML/Graphics.hpp>
#include "camera_impl.hpp"
#include "shared_constants.hpp"
#include "event_dispatcher.hpp"
class ProgressBar{
    std::size_t progress;
    std::size_t total;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::RectangleShape bar_container;
    sf::RectangleShape bar;
    sf::Text text;
public:
    ProgressBar(sf::FloatRect position,sf::Font* font);
    void update(std::size_t progress_now,std::size_t total,std::size_t char_length);
    void draw(sf::RenderWindow& rw);
};
//Add About Section
class AboutSection{
    sf::RectangleShape rect;
    sf::Text text;
public:
    bool show_gui;
    AboutSection(const char* str,sf::Font* font,sf::FloatRect rect);
    void draw(sf::RenderWindow& rw);
};
using ButtonCallback=void(* )(AnyData);
constexpr void NULLCallback(AnyData /*ad*/){}
class GUIButton{
    sf::RectangleShape rect;
    sf::Text text;
    ButtonCallback callback;
    AnyData any_data;
public:
    GUIButton(const char* str,sf::Font* font,sf::FloatRect rect);
    void update(sf::Event& mouse_event);
    void new_callback(ButtonCallback bcb,AnyData any_data);
    void draw(sf::RenderWindow& rw);
};
class Minimap{
	std::vector<sf::Vertex> vbuffer;
	std::vector<std::tuple<std::size_t,std::size_t,sf::PrimitiveType>> properties;
public:
	Minimap();
	void add(const sf::Vertex* buffer,std::size_t size,sf::PrimitiveType type);
	void draw(sf::RenderWindow& r_window,Camera& camera);
};
#endif