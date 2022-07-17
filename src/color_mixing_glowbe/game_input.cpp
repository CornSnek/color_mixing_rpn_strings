#include "game_input.hpp"

GameInput::GameInput():key_table(){}
void GameInput::set_key_down(InputType k){
    for(const auto& dup_k:key_table) if(dup_k==k) return; //Don't add the same key.
    key_table.push_back(k);
}
void GameInput::set_key_up(InputType k){
    for(std::size_t erase_i=0;erase_i!=key_table.size();erase_i++)
        if(key_table.at(erase_i)==k){
            key_table.erase(key_table.begin()+erase_i); //Remove element between vector.
            return; //End as vectors can uninitialize current memory.
        }
}
bool GameInput::is_pressed(InputType k) const{
    for(const auto& compare_k:key_table) if(compare_k==k) return true;
    return false;
}
//Base case for variadic template (no args).
bool GameInput::any_pressed() const{
    return false;
}
bool GameInput::not_pressed(InputType k) const{
    for(const auto& compare_k:key_table) if(compare_k==k) return false;
    return true;
}
/*Copy pressed inputs into a vector todo switch/case of multiple game inputs.*/
GameInputVec GameInput::get_inputs() const{
    GameInputVec key_v(key_table.size());
    std::copy(key_table.cbegin(),key_table.cend(),key_v.begin());
    return key_v;
}
void GameInput::print_keys() const{
    std::cout<<"[";
    for(const auto& key:key_table) std::cout<<key<<",";
    std::cout<<"]\n";
}
GameInput::~GameInput(){}