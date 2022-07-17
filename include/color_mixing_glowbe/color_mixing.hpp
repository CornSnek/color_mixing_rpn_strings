#ifndef _COLOR_MIXING_HPP_
#define _COLOR_MIXING_HPP_
#include <stdexcept>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <span>
#include "generators.hpp"
#include "shared_constants.hpp"
/*
ColorMidpointInfix tries to follow the commutative property and non-associative property of
the midpoint formula (In infix notation).
Midpoint of A=(w,x) B=(y,z), midpoint(A,B)=((w+y)/2,(x+z)/2).
The class also tries to sort in alphabetical order.
Also the class merges similar points together, Ex: midpoint(A,A)=((w+w)/2,(x+x)/2)=(w,x)=A.
*/
class ColorMidpointInfix{
public:
	ColorMidpointInfix();
	ColorMidpointInfix(char ch);
	ColorMidpointInfix(const char* c1,const char* c2);
	ColorMidpointInfix(const ColorMidpointInfix& cm_i);
	~ColorMidpointInfix();
	void print_char_array() const;
	[[nodiscard("Memory leak. Requires delete[].")]] const char* get_value();
	ColorMidpointInfix& operator=(const ColorMidpointInfix& cm_i);
	ColorMidpointInfix operator+(const ColorMidpointInfix& other);
private:
	char* value;
};
/*
ColorMidpointRPN tries to follow the commutative property and non-associative property of
the midpoint formula in Reverse Polish Notation or postfix notation.
Ex: Points A,B,C where '+' is midpoint function, then A+B=['A','B','+'],
A+(B+C)=['A','B','C','+','+'], A+A=['A'] since midpoint of A and A is just the same point.
Note: Operands can be arranged in alphabetical order if shown in infix notation
due to midpoint commutative property.
 */
class ColorMidpointRPN{
public:
	ColorMidpointRPN();
	ColorMidpointRPN(char ch);
	ColorMidpointRPN(const ColorMidpointRPN& cm_rpn1, const ColorMidpointRPN& cm_rpn2);
	ColorMidpointRPN(const ColorMidpointRPN& cm_r);
	~ColorMidpointRPN();
	ColorMidpointRPN& operator=(const ColorMidpointRPN& cm_r);
	ColorMidpointRPN& operator=(ColorMidpointRPN&& cm_r);
	ColorMidpointRPN operator+(const ColorMidpointRPN& other) const;
	void print_char_array() const;
	[[nodiscard("Memory leak. Requires delete[].")]] const char* copy_to_c_str() const;
	friend std::pair<bool,ColorMidpointRPN> verify_unmerged_rpn(const std::span<char>& ms);
private:
	char* value;
	size_t length;
};
struct BRYVector{
	int B;
	int R;
	int Y;
	int D;//Denominator.
	BRYVector();
	BRYVector(unsigned char ch);
	BRYVector(int B,int R,int Y,int D):B(B),R(R),Y(Y),D(D){}//For debugging purposes.
	BRYVector operator+(const BRYVector& other) const;
	BRYVector operator*(int mult) const;
	BRYVector& operator+=(const BRYVector& other);
	bool operator==(const BRYVector& other) const;
	void print() const;
	unsigned long long to_hash() const;
	void simplify();
	void get_fractions(std::string& B_s,std::string& R_s,std::string& Y_s) const;
};
BRYVector rpn_to_color(const std::span<char>& rpn_c,const std::span<char>& char_span,const std::span<BRYVector>& char_color_span);
BRYVector rpn_to_bryvector(const char* rpn_c);
ColorMidpointInfix rpn_to_infix(const char* rpn_c);
BRYVector rpn_to_bryvector(const std::span<char>& rpn_c,const std::span<char>& char_span,const std::span<BRYVector>& bry_colors_span);
class BRYColorWheel{
	BRYVector bryv;
	public:
		BRYColorWheel() noexcept:bryv(){}
		BRYColorWheel(const BRYVector& bryv) noexcept:bryv(bryv){}
		unsigned int get_color(unsigned char alpha=0xff) const;
};
#endif