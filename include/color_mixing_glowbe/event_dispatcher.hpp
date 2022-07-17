#ifndef _EVENT_DISPATCHER_HPP_
#define _EVENT_DISPATCHER_HPP_
#include <vector>
#include <iostream>
#include <SFML/Window/Event.hpp>
using EventObjectID=int;
using AnyData=void*;
enum class EventType{
    Invalid,
    HoverTriangle
};
namespace EventDataShare{
    struct WorldCoords{
        float x;
        float y;
        float zoom;
        sf::Event mouse_event;
    };
    union Data{
        sf::Event sfml_event; //Temporary
        WorldCoords world_coords;
    };
};
struct EventData{
    EventType event_type;
    EventDataShare::Data data;
    EventData(EventType event_type,EventDataShare::Data data):event_type(event_type),data(data){}
};
static const EventObjectID EVENT_DISCONNECTED=-1;
template<class T_Class>
class EventFor{
public:
    EventFor();
    virtual ~EventFor();
    static EventObjectID register_obj(T_Class* this_obj);
    static bool deregister_obj(EventObjectID id);
    EventObjectID get_obj_id();
    virtual void call(const EventData& ud); //From dispatch_event
    static void dispatch_event(const EventData& ud);
    std::size_t get_num_connected();
protected:
    EventObjectID obj_id;
    static EventObjectID reg_i;
    static std::size_t num_connected;
    static std::vector<EventFor<T_Class>*> reg_objs;
    static EventFor<T_Class> nullobj_t; //As "nullptr" for reg_objs.
};
#include <event_dispatcher.inl>
#endif