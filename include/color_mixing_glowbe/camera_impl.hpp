#ifndef _CAMERA_IMPL_HPP_
#define _CAMERA_IMPL_HPP_
#include <cmath>
#include <vector>
#include <iostream>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "event_dispatcher.hpp"
#include "game_input.hpp"
#include "rpn_container.hpp"
#include "shared_constants.hpp"
class CameraBounds;
/*"Simple" Zoom camera using SFML View class.*/
class Camera{
public:
    Camera(float screen_width,float screen_height);
    ~Camera();
    void input_update(const GameInput& giv);
    void set_to_world_view(sf::RenderWindow& r_window);
    bool world_view_can_draw(sf::FloatRect rect_bounds);
    void set_to_gui_view(sf::RenderWindow& r_window);
    sf::View get_minimap_view(sf::FloatRect rect_minimap,sf::FloatRect rect_bounds);
    void set_camera_bounds(CameraBounds*&& camera_bounds);
    void mouse_to_world(sf::Event& mouse_event);
    sf::Vector2f get_world_coords() const;
    void check_cursor(sf::CircleShape& cs);
    std::string get_camera_info();
    CameraBounds get_camera_bounds();
    float get_zoom_scale();
    void get_camera_properties(sf::Vector2f& camera_center,float& zoom_scale, float& rotation);
    friend CameraBounds;
private:
    const float screen_width;
    const float screen_height;
    sf::Vector2f camera_center;
    float zoom_scale;
    float rotation;
    sf::Vector2f world_coords;
    CameraBounds* camera_bounds;
};
class CameraBounds{
public:
    const float left_bounds;
    const float top_bounds;
    const float right_bounds;
    const float bottom_bounds;
    const float min_zoom;
    const float max_zoom;
    explicit CameraBounds(const Camera& camera,const sf::FloatRect& bound_box,
        float min_zoom, float max_zoom);
};
#endif