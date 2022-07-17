#ifndef _FPS_COUNT_HPP_
#define _FPS_COUNT_HPP_
#include <ctime>
#include <SFML/System/Clock.hpp>
class FPSCount{
public:
    FPSCount():clock(),fps_count(0),fps(0){}
    void update(){
        fps_count++;
        if(clock.getElapsedTime().asMilliseconds()>=1000){
            fps=fps_count;
            fps_count=0;
            clock=sf::Clock();
        }
    }
    std::size_t get_fps(){
        return fps;
    }
private:
    sf::Clock clock;
    std::size_t fps_count;
    std::size_t fps;
};
#endif