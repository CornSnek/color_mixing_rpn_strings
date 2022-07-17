template<class T_Class>//static
EventObjectID EventFor<T_Class>::reg_i=0;//Points at "free space" or "null-object" index.
template<class T_Class>//static
std::size_t EventFor<T_Class>::num_connected=0;//Counts how many T_Classes are connected.
template<class T_Class>//static
std::vector<EventFor<T_Class>*> EventFor<T_Class>::reg_objs{};
template<class T_Class>//static
EventFor<T_Class> EventFor<T_Class>::nullobj_t{};

template<class T_Class>
EventFor<T_Class>::EventFor():obj_id(EVENT_DISCONNECTED){}
template<class T_Class>
EventFor<T_Class>::~EventFor(){
    if(obj_id==EVENT_DISCONNECTED) return;//Don't destroy in vector if not registered.
    //std::cout<<"Deregistering obj_id #"<<obj_id<<std::endl;
    deregister_obj(obj_id);
}
template<class T_Class>//static
EventObjectID EventFor<T_Class>::register_obj(T_Class* this_obj){
    if(this_obj->obj_id!=EVENT_DISCONNECTED) return EVENT_DISCONNECTED;//Can't connect already registered objects.
    do this_obj->obj_id=reg_i++;
    while(this_obj->obj_id!=static_cast<int>(reg_objs.size())&&(reg_objs.at(this_obj->obj_id)->obj_id!=EVENT_DISCONNECTED)); //Don't override connected ones.
    if(this_obj->obj_id==static_cast<int>(reg_objs.size())) reg_objs.push_back(this_obj);
    else reg_objs.at(this_obj->obj_id)=this_obj;
    //std::cout<<"Registering obj_id #"<<this_obj->obj_id<<std::endl;
    num_connected++;
    return this_obj->obj_id;
}
template<class T_Class>//static
bool EventFor<T_Class>::deregister_obj(EventObjectID del_id){
    if(del_id==EVENT_DISCONNECTED) return false;//No disconnected objects.
    //std::cout<<"Deregistering obj_id #"<<del_id<<std::endl;
    reg_i=std::min(reg_i,del_id);//Lowest reg_i here for EVENT_DISCONNECTED T_Class (obj_id=EVENT_DISCONNECTED) to be rewritten by other T_Classes.
    reg_objs.at(del_id)->obj_id=EVENT_DISCONNECTED;//Set as disconnected.
    if(del_id!=static_cast<EventObjectID>(reg_objs.size()-1))
        reg_objs.at(del_id)=&nullobj_t;
    else reg_objs.pop_back(); //Make shorter or create a "null object"
    num_connected--;
    return true;
}
template<class T_Class>
EventObjectID EventFor<T_Class>::get_obj_id(){
    return obj_id;
}
template<class T_Class>
void EventFor<T_Class>::call(const EventData& /*ud*/){}
template<class T_Class>//static
void EventFor<T_Class>::dispatch_event(const EventData& ud){
    for(const auto& obj:reg_objs){
        obj->call(ud);//Virtual polymorphic call()
    }
}
template<class T_Class>//static
std::size_t EventFor<T_Class>::get_num_connected(){
    return num_connected;
}