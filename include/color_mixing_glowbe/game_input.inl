template<typename... OtherInputTypes>
/*True if either one key is pressed*/
bool GameInput::any_pressed(InputType k,OtherInputTypes...k_others) const{
    return is_pressed(k)||any_pressed(k_others...);
}