#ifndef _RPN_STACK_HPP_
#define _RPN_STACK_HPP_
#include <cstddef>
//Stack specialized for RPN. No "safety" checks but just delete when out of scope.
template<class T_Container>
class RPNStack{
public:
    RPNStack(std::size_t rpn_size);
    ~RPNStack();
    void push(T_Container& t_obj);
    void push(T_Container&& t_obj);
    T_Container pop();
    void combine_two();
    std::pair<T_Container,T_Container> pop_two();
private:
    std::size_t stack_i;
    T_Container* t_stack;
};
template<class T_Container>
RPNStack<T_Container>::RPNStack(std::size_t rpn_size)
    :stack_i(0),t_stack(new T_Container[(rpn_size/2)+1]){
}//(rpn_size/2)+1 to count maximum number of operands from rpn_size.
template<class T_Container>
RPNStack<T_Container>::~RPNStack(){
    delete[] t_stack;
}
template<class T_Container>
void RPNStack<T_Container>::push(T_Container& t_obj){
    t_stack[stack_i++]=t_obj;
}
template<class T_Container>
void RPNStack<T_Container>::push(T_Container&& t_obj){
    t_stack[stack_i++]=t_obj;
}
template<class T_Container>
T_Container RPNStack<T_Container>::pop(){
    return t_stack[--stack_i];
}
template<class T_Container>
void RPNStack<T_Container>::combine_two(){
    t_stack[stack_i-2]=t_stack[stack_i-1]+t_stack[stack_i-2];
    stack_i--;
}
template<class T_Container>
std::pair<T_Container,T_Container> RPNStack<T_Container>::pop_two(){
    auto return_this=std::pair<T_Container,T_Container>(t_stack[stack_i-1],t_stack[stack_i-2]);
    stack_i-=2;
    return return_this;
}
#endif