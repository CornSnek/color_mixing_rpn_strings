#ifndef _SHARED_CONSTANTS_
#define _SHARED_CONSTANTS_
#include <SFML/Graphics.hpp>
#include <cmath>
#define __PI 3.141592653589793f
//From main.cpp
constexpr int ScreenWidth=1200;
constexpr int ScreenHeight=960;
constexpr std::size_t MaxCharLength=10; //Greater than 2. Don't touch.
constexpr float TriLen = 3500.f;
constexpr float LongTriLen(){
	return TriLen*std::sqrt(3.0f)/2.0f;
}
constexpr float ScreenRatioAngle(){
    return std::atan(static_cast<float>(ScreenHeight)/ScreenWidth)/__PI*180;
}
constexpr float ScreenRatioHypLen(){
    return std::sqrt(ScreenWidth*ScreenWidth+ScreenHeight*ScreenHeight);
}
constexpr float ToRad(float deg){
    return deg*__PI/180;
}
constexpr sf::Int32 FPMS=static_cast<sf::Int32>((1.0f/60.0f)*1000.0f);
//From color_mixing.cpp
enum class Colors{B,R,Y};
namespace CEnums{
    constexpr float b[3]={0,0,255},r[3]={255,0,0},y[3]={255,255,0}
        ,gr[3]={128,128,128},g[3]={0,255,0},m[3]={255,0,255};
}
constexpr const char* MixNames[]={
    "","primary","secondary","tertiary"
    ,"quaternary","quinary","senary"
    ,"septenary","octonary","nonary","denary"};//0th as ""
constexpr const char* MixNamesCapitals[]={
    "","Primary","Secondary","Tertiary"
    ,"Quaternary","Quinary","Senary"
    ,"Septenary","Octonary","Nonary","Denary"};
constexpr const char* AboutSectionStr=
"This program was inspired by Glowbes from My Singing Monsters.\n"
"From https://mysingingmonsters.fandom.com/wiki/Glowbes. I was inspired\n"
"by the math of the color combinations of Glowbes, I have created a program\n"
"that creates Reverse Polish Notation (RPN), or Postfix strings of Blue, Red,\n"
"and Yellow glowbes to find different color combinations of Primary, Secondary, Tertiary,\n"
"all the way to Denary Glowbes (x10 Glowbe combinations).\n"
"Controls: W,A,S,D to move the camera up, left, down, and right. Q and E to zoom in/out.\n"
"Z and C to rotate the camera clockwise and counter-clockwise. Click on any rotated\n"
"triangle to see its color combination. You will have to zoom in a triangle until it rotates.\n"
"Math:\n"
"The + operator is the midpoint of the vectors B[1,0,0], R[0,1,0], and Y[0,0,1].\n"
"The triangle vectors/points all lie on the plane b+r+y=1.\n"
"The string format is in RPN notation. Examples: BR+ is read as mix Blue and Red. BBR+BY+++ is\n"
"read as \"Mix Blue+Red, then Yellow+Blue, then mix Blue+Red with Blue+Yellow, and finally\n"
"mix Blue with the Blue+Red and Yellow+Blue combination\". More information of RPN at\n"
"https://mathworld.wolfram.com/ReversePolishNotation.html. Note that there is also\n"
"infix notation using parenthesis on the right of the RPN string.\n"
"Some interesting math properties of midpoints and the B/R/Y vectors is that\n"
"it is not associative. Example: (BR)Y =/= B(RY). It is commutative,\n"
"BY == YB and (BY)R == (YB)R == R(YB) == R(BY). Any same color combination can be\n"
"merged together or simplify. Example: (BB)R == BR, ((YB)(BY))(RY) == (YB)(RY).\n"
"The program uses multiset strings and dyck words to find all the color combinations.\n"
"It will only insert strings if the string does not merge together or simplify\n"
"and it will only insert if it is the lowest combination it can be for a triangle.\n"
"Example: If a triangle is only Tertiary, it will not add\n"
"any higher combinations of Quaternary, Quinary, Senary... etc. strings.\n"
"This program was made using SFML 2.5.1 and C++20 in Visual Studio Code.\n"
"Font used: Kenney Pixel.ttf by Kenney https://www.kenney.nl/";
#endif