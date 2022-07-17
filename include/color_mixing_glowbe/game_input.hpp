#ifndef GAMEINPUT_H_
#define GAMEINPUT_H_
#include <cstdarg>
#include <vector>
#include <iostream>
#include <SFML/Window/Keyboard.hpp>

//If InputType is a primitive.
using InputType=sf::Keyboard::Key;
using GameInputVec=std::vector<InputType>;
/*
 * Gets multiple key inputs for use to update entities.
 **/
class GameInput{
public:
    GameInput();
    ~GameInput();
    void set_key_down(InputType k);
    void set_key_up(InputType k);
    bool is_pressed(InputType k) const;
    template<typename... OtherInputTypes>
    bool any_pressed(InputType k,OtherInputTypes...k_others) const;
    bool any_pressed() const;
    bool not_pressed(InputType k) const;
    GameInputVec get_inputs() const;
    void print_keys() const;
private:
    GameInputVec key_table;
};
#include "game_input.inl"
#endif