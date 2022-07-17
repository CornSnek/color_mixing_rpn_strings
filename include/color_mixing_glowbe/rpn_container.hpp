#ifndef _RPN_CONTAINER_HPP_
#define _RPN_CONTAINER_HPP_
#include <cstring>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <span>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "color_mixing.hpp"
#include "camera_impl.hpp"
#include "rpn_stack.hpp"
#include "event_dispatcher.hpp"
#include "shared_constants.hpp"

class Camera;
class ColorSprite;
struct HashForBRYVector{
    unsigned int operator()(const BRYVector& bry_v) const{
        return bry_v.to_hash();
    }
};
using RPNVector=std::vector<const char*>;
/*Only unique BRYVectors for maps below*/
using RPNMap=std::unordered_map<BRYVector,RPNVector,HashForBRYVector>;
using RPNColorSprites=std::unordered_map<BRYVector,ColorSprite,HashForBRYVector>;

/*Is (* owner) and container of all RPN (const char*) strings*/
class RPNContainer{
public:
    RPNContainer();
    RPNContainer(const RPNContainer& to_copy)=delete;//No copying.
    RPNContainer& operator=(const RPNContainer& to_copy)=delete;//No assignment copying.
    RPNContainer(RPNContainer&& to_move);//Just move constructor.
    ~RPNContainer();
    bool add_if_unique(BRYVector bry_v,ColorMidpointRPN& cmr);
    void add_char(BRYVector bry_v,const char c);
    void copy_map_to_parse();
    void add_color_sprites(bool& done,RPNColorSprites& container,const std::span<char>& char_span,const std::span<sf::Vector2f>& char_vector_span,sf::Font*& shared_font);
    RPNVector* get_rpnvector_by(BRYVector bry_v);
    void write_to_file(std::ofstream& output);
    std::size_t total_colors();
    std::size_t total_inserted();
private:
    /*Keys comparing with HashForBRYVector to get color uniqueness.*/
    RPNMap uom;
    RPNMap uom_to_parse;
    std::size_t _total_inserted;
    static std::mutex rw_mutex;
    static std::mutex parse_mutex;
    static std::condition_variable parse_hv;
    friend ColorSprite; //To share rw_mutex when any RPNVector is accessed while writing.
};
/*Lifetime of RPNVector& is from RPNContainer::uom*/
class ColorSprite:public EventFor<ColorSprite>{
public:
    ColorSprite(float min_zoom,sf::Vector2f world_position,BRYVector bry_v,sf::Font* font);
    ~ColorSprite() override;
    bool draw_to_window(sf::RenderWindow& rw,Camera& cam,RPNContainer& rpn_c);
    static void sprite_update(Camera& cam);
    void call(const EventData& ud) override;
private:
    static sf::Text* last_with_text;
    static float rotation;
    static const float triangle_side;
    bool show_rpns;
    float min_draw_zoom;
    sf::ConvexShape triangle;
    sf::Vector2f world_position;
    BRYVector bry_v;
    sf::Text text;
};
#endif