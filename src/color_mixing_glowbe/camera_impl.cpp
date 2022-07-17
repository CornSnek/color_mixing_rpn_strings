#include "camera_impl.hpp"

Camera::Camera(float screen_width,float screen_height)
    :screen_width(screen_width),screen_height(screen_height)
    ,camera_center(),zoom_scale(1.0f),rotation(0.0f),world_coords(),camera_bounds(nullptr){}
Camera::~Camera(){delete camera_bounds;}
//#defines for x, y, and zoom_scale bounds (when zooming out).
#define __TopmostCenterPoint camera_bounds->top_bounds+(screen_height/2.0f)*zoom_scale
#define __BottommostCenterPoint camera_bounds->bottom_bounds-(screen_height/2.0f)*zoom_scale
#define __LeftmostCenterPoint camera_bounds->left_bounds+(screen_width/2.0f)*zoom_scale
#define __RightmostCenterPoint camera_bounds->right_bounds-(screen_width/2.0f)*zoom_scale
//#defines to move by screen height/width (without zoom multiplier), or zoom 2x .5x per FPS.
#define __ScrHM (static_cast<float>(screen_height)/60.f)
#define __ScrWM (static_cast<float>(screen_width)/60.f)
#define __ZoomOut powf(2.0f,1.0f/60.f)
#define __ZoomIn powf(0.5f,1.0f/60.f)
#define __RotateBy 90.f/60.f
void Camera::input_update(const GameInput& gi){
    float shift_mult=gi.any_pressed(sf::Keyboard::RShift,sf::Keyboard::LShift)?3.0f:1.0f;
    for(const auto& k:gi.get_inputs()){
        switch(k){
        case sf::Keyboard::W:
            if(camera_bounds) camera_center.y=std::max(camera_center.y-__ScrHM*zoom_scale,__TopmostCenterPoint);
            else camera_center.y-=__ScrHM*zoom_scale;
            break;
        case sf::Keyboard::S:
            if(camera_bounds) camera_center.y=std::min(camera_center.y+__ScrHM*zoom_scale,__BottommostCenterPoint);
            else camera_center.y+=__ScrHM*zoom_scale;
            break;
        case sf::Keyboard::A:
            if(camera_bounds) camera_center.x=std::max(camera_center.x-__ScrWM*zoom_scale,__LeftmostCenterPoint);
            else camera_center.x-=__ScrWM*zoom_scale;
            break;
        case sf::Keyboard::D:
            if(camera_bounds) camera_center.x=std::min(camera_center.x+__ScrWM*zoom_scale,__RightmostCenterPoint);
            else camera_center.x+=__ScrWM*zoom_scale;
            break;
        case sf::Keyboard::Q:
            if(camera_bounds) zoom_scale=std::max(zoom_scale*__ZoomIn,camera_bounds->min_zoom);
            else zoom_scale*=__ZoomIn;
            break;
        case sf::Keyboard::E:
            if(camera_bounds){//Readjust camera_center as well when zooming out.
                zoom_scale=std::min(zoom_scale*__ZoomOut,camera_bounds->max_zoom);
                camera_center.x=std::clamp(camera_center.x,__LeftmostCenterPoint,__RightmostCenterPoint);
                camera_center.y=std::clamp(camera_center.y,__TopmostCenterPoint,__BottommostCenterPoint);
            }
            else zoom_scale*=__ZoomOut;
            break;
        case sf::Keyboard::Z:
            rotation=std::fmod(rotation-__RotateBy*shift_mult,360.f);
            break;
        case sf::Keyboard::C:
            rotation=std::fmod(rotation+__RotateBy*shift_mult,360.f);
            break;
        case sf::Keyboard::R: camera_center.x=0.0f; camera_center.y=0.0f; zoom_scale=1.0f; rotation=0.0f; break;
        default: break;
        }
    }
}
void Camera::set_to_world_view(sf::RenderWindow& r_window){
    sf::View return_this(sf::View(camera_center,sf::Vector2f(screen_width*zoom_scale,screen_height*zoom_scale)));
    return_this.setRotation(rotation);
    r_window.setView(return_this);
}
void Camera::set_to_gui_view(sf::RenderWindow& r_window){
    r_window.setView(sf::View(sf::FloatRect(0.0,0.0,screen_width,screen_height)));
}
sf::View Camera::get_minimap_view(sf::FloatRect rect_minimap,sf::FloatRect rect_bounds){//TODO
    sf::View return_this;
    return_this.reset(rect_minimap);
    return_this.setViewport(sf::FloatRect(rect_minimap.left/screen_width
        ,rect_minimap.top/screen_height
        ,rect_minimap.width/screen_width
        ,rect_minimap.height/screen_height));
    return_this.setCenter(0,0);
    const float s1=rect_bounds.width/rect_minimap.width;
    const float s2=rect_bounds.height/rect_minimap.height;
    return_this.zoom(s1>s2?s1:s2);//Whichever is greater to fill rect_bounds.
    return return_this;
}
bool Camera::world_view_can_draw(sf::FloatRect rect_bounds){
    sf::FloatRect camera_rect(sf::Vector2f(camera_center.x-(screen_width/2.0f)*zoom_scale,camera_center.y-(screen_height/2.0f)*zoom_scale),
        sf::Vector2f(screen_width*zoom_scale,screen_height*zoom_scale));
    return rect_bounds.intersects(camera_rect);
}
/*Camera will be * owner of CameraBounds and delete when destructed or reassigned*/
void Camera::set_camera_bounds(CameraBounds*&& camera_bounds){
    delete this->camera_bounds;
    this->camera_bounds=camera_bounds;
}
//Defines to convert mouse coordinates to relative distance of camera's center
//Including rotation and zoom.
#define __ROTCOS cosf(rotation*__PI/180.0f)
#define __ROTSIN sinf(rotation*__PI/180.0f)
#define __RELX (scr_x-screen_width/2.0f)
#define __RELY (scr_y-screen_height/2.0f)
#define __MCX (((__RELX*__ROTCOS+__RELY*-__ROTSIN)*zoom_scale)+camera_center.x)
#define __MCY (((__RELX*__ROTSIN+__RELY*__ROTCOS)*zoom_scale)+camera_center.y)
void Camera::mouse_to_world(sf::Event& mouse_event){
    float scr_x,scr_y; //Used for defines.
    switch(mouse_event.type){
    case(sf::Event::MouseMoved):
        scr_x=mouse_event.mouseMove.x; scr_y=mouse_event.mouseMove.y; break;
    case(sf::Event::MouseButtonPressed): [[fallthrough]];
    case(sf::Event::MouseButtonReleased):
        scr_x=mouse_event.mouseButton.x; scr_y=mouse_event.mouseButton.y; break;
    default: throw std::logic_error("Event not implemented for Camera::mouse_to_world");
    }
    world_coords=sf::Vector2f(__MCX,__MCY);
    EventFor<ColorSprite>::dispatch_event(EventData(EventType::HoverTriangle,EventDataShare::Data{.world_coords{.x=world_coords.x,.y=world_coords.y,.zoom=get_zoom_scale(),.mouse_event=mouse_event}}));
}
sf::Vector2f Camera::get_world_coords() const{
    return world_coords;
}
void Camera::check_cursor(sf::CircleShape& cs){
    cs.setPosition(world_coords);
}
#define __TS(_str) std::to_string(_str)
std::string Camera::get_camera_info(){
    return std::string("Camera Center: [")+__TS(camera_center.x)+","+__TS(camera_center.y)+"]\nZoom Scale: "+__TS(zoom_scale)+"]\nRotation (Counter clockwise w/ respect to Camera): "+__TS(rotation)+"\nWorld coordinates: "+__TS(world_coords.x)+","+__TS(world_coords.y)+"\n";
}
CameraBounds Camera::get_camera_bounds(){
    if(!camera_bounds) throw std::logic_error("Camera bounds has not been initialized.");
    return *camera_bounds;
}
float Camera::get_zoom_scale(){
    return zoom_scale;
}
void Camera::get_camera_properties(sf::Vector2f& camera_center,float& zoom_scale, float& rotation){
    camera_center=this->camera_center;
    zoom_scale=this->zoom_scale;
    rotation=this->rotation;
}
#define __min_3way(m1,m2,m3) std::min(m1,std::min(m2,m3))
/* Bounds the camera will be set to. Note: max_zoom will be set to either the minimum of set value or
the minimum of the ratio of the screen size of either the current screen width or height. */
CameraBounds::CameraBounds(const Camera& camera,const sf::FloatRect& bound_box,
        float min_zoom, float max_zoom)
    :left_bounds(bound_box.left),top_bounds(bound_box.top)
    ,right_bounds(bound_box.width),bottom_bounds(bound_box.height)
    ,min_zoom(min_zoom)
    ,max_zoom(__min_3way(max_zoom,(right_bounds-left_bounds)/camera.screen_width,(bottom_bounds-top_bounds)/camera.screen_height)
){}