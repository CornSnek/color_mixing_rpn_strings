#include <cstring>
#include <cstdio>
#include "color_mixing.hpp"

//nullptr so that destructor is not deleting an uninitialized pointer
ColorMidpointInfix::ColorMidpointInfix():value(nullptr){}
ColorMidpointInfix::ColorMidpointInfix(char ch):value(new char[2]()){
	value[0]=ch;
	value[1]='\0';
}

//Assuming ch_a1 and ch_a2 are null-terminated ('\0') only at the end of the array.
ColorMidpointInfix::ColorMidpointInfix(const char* c1,const char* c2):value(nullptr)
{
	std::size_t size=strlen(c1)+strlen(c2)+3;//+3 to enclose with '(', ')' chars and add '\0'
	value=new char[size]();
	value[0]='(';
	strcpy(value+1,c1);
	strcpy(value+1+strlen(c1),c2);
	value[size-2]=')';
	value[size-1]='\0';
}
ColorMidpointInfix::ColorMidpointInfix(const ColorMidpointInfix& cm_i)
	:value(new char[strlen(cm_i.value)+1]()){
	strcpy(this->value,cm_i.value);
	this->value[strlen(cm_i.value)]='\0';
}
ColorMidpointInfix::~ColorMidpointInfix(){
	delete[] value;//Delete memory when allocating previously.
}
void ColorMidpointInfix::print_char_array() const{
	char* it=value;
	/*printf("[");
	while(*it!='\0'){
		printf("'%c',",*it);
		it++;
	}
	printf("'\\0']\n");*/
	while(*it){
		printf("%c",*it);
		it++;
	}
	printf("\n");
}
[[nodiscard]] const char* ColorMidpointInfix::get_value(){
	char* return_this=new char[strlen(value)+1]();
	strcpy(return_this,value);
	return_this[strlen(value)]='\0';
	return return_this;
}
ColorMidpointInfix& ColorMidpointInfix::operator=(const ColorMidpointInfix& cm_i){
	if(this!=&cm_i){//If not self.
		std::size_t size=strlen(cm_i.value)+1;
		delete[] this->value; //Delete old contents to be replaced by new allocation.
		this->value=new char[size]();
		strcpy(this->value,cm_i.value);
		this->value[size-1]='\0';
	}
	return *this;
}
inline char* non_parenthesis(char* c_p){while(*c_p=='('){c_p++;} return c_p;}
ColorMidpointInfix ColorMidpointInfix::operator+(const ColorMidpointInfix& other){
	if(strcmp(this->value,other.value)==0){
		return *this;//"Merge" the midpoints as they have the same elements.
	}else{//Make char array in lexicographical or alphabetical order (excluding parenthesis).
		/*char* t1=non_parenthesis(this->value);
		char* t2=non_parenthesis(other.value);
		printf("|%c,%c| vs |%c,%c|\n",this->value[0],other.value[0],t1[0],t2[0]);*/
		int lex=strcmp(non_parenthesis(this->value),non_parenthesis(other.value));//Compare excluding parentheses.
		if(lex>0) return ColorMidpointInfix(other.value,this->value);
		return ColorMidpointInfix(this->value,other.value);
	}
}

ColorMidpointRPN::ColorMidpointRPN():value(nullptr),length(0){}
ColorMidpointRPN::ColorMidpointRPN(char ch):value(new char[1]()),length(1){
	value[0]=ch;
}
ColorMidpointRPN::ColorMidpointRPN(const ColorMidpointRPN& cm_rpn1, const ColorMidpointRPN& cm_rpn2)
	:value(new char[cm_rpn1.length+cm_rpn2.length+1]())
	,length(cm_rpn1.length+cm_rpn2.length+1)//+1 to add '+' operator
{
	strncpy(value,cm_rpn1.value,this->length);
	strncpy(value+cm_rpn1.length,cm_rpn2.value,cm_rpn2.length);//Pointer offset by cm_rpn1's length
	value[this->length-1]='+';
}
ColorMidpointRPN::ColorMidpointRPN(const ColorMidpointRPN& cm_r)
	:value(new char[cm_r.length]()),length(cm_r.length){
	strncpy(this->value,cm_r.value,cm_r.length);
}
ColorMidpointRPN::~ColorMidpointRPN(){
	//printf("Deleting this: "); this->print_char_array();
	delete[] value;
}
ColorMidpointRPN& ColorMidpointRPN::operator=(const ColorMidpointRPN& cm_r){
	if(this!=&cm_r){//If not self.
		this->length=cm_r.length;
		//printf("Deleting this: "); this->print_char_array();
		delete[] this->value; //Delete old allocated contents.
		this->value=new char[cm_r.length]();
		strncpy(this->value,cm_r.value,cm_r.length);
	}
	return *this;
}
ColorMidpointRPN& ColorMidpointRPN::operator=(ColorMidpointRPN&& cm_r){
	std::swap(this->value,cm_r.value);
	std::swap(this->length,cm_r.length);
	return *this;
}
inline const char* as_null_term_str(const char* to_copy,std::size_t len){
	char* return_this=new char[len+1](); //+1 to add '/0'
	strncpy(return_this,to_copy,len);
	return_this[len]='\0';
	return return_this;
}
ColorMidpointRPN ColorMidpointRPN::operator+(const ColorMidpointRPN& other) const{
	ColorMidpointRPN return_this;
	const char* this_value=as_null_term_str(this->value,this->length);
	const char* other_value=as_null_term_str(other.value,other.length);
	const int strcmp_num=strcmp(this_value,other_value);
	if(!strcmp_num){
		return_this=*this;//"Merge" the midpoints as they have the same elements.
	}else if(strcmp_num>0){//+ and - numbers swap left-right operands in lexicographical order.
		return_this=ColorMidpointRPN(other,*this);
	}else{
		return_this=ColorMidpointRPN(*this,other);
	}
	/*switch(strcmp(this_value,other_value)){//Fixed bug: Null-terminate the strings for strcmp to work properly.
	case 0:
		return_this=*this;//"Merge" the midpoints as they have the same elements.
		break;
	case 1://cases 1 and -1 to swap left-right operands in lexicographical order.
		return_this=ColorMidpointRPN(other,*this);
		break;
	case -1:
		return_this=ColorMidpointRPN(*this,other);
		break;
	default:
		printf("%d\n",strcmp(this_value,other_value));
		throw std::logic_error("strcmp should only implement 0, 1, and -1.");
	}*/
	delete[] this_value;
	delete[] other_value;
	return return_this;
}
void ColorMidpointRPN::print_char_array() const{
	for(std::size_t i=0;i!=length;i++){
		printf("%c",value[i]);
	}
	printf("\n");
}
/*[[nodiscard]]: Requires const char* value delete[].*/
[[nodiscard]] const char* ColorMidpointRPN::copy_to_c_str() const{
	auto return_this=new char[length+1]();//+1 for '\0'
	strncpy(return_this,value,length);
	return_this[length]='\0';
	return return_this;
}


#define __MidP(member) (this->member+other.member)/2
#define __AsUInt(u_ch) static_cast<unsigned int>(u_ch)
BRYVector::BRYVector():B(0),R(0),Y(0),D(0){}
BRYVector::BRYVector(unsigned char ch):B(0),R(0),Y(0),D(1){
	switch(ch){
		case 'B': B=1; break;
		case 'R': R=1; break;
		case 'Y': Y=1; break;
		default: throw std::logic_error("Only 'B', 'Y', and 'R' allowed.");
	}
}
#define __BRYAdd1(member) new_bry.member=(this->member*multiply_d)+other.member
#define __BRYAdd2(member) new_bry.member=(other.member*multiply_d)+this->member
BRYVector BRYVector::operator+(const BRYVector& other) const{
	int multiply_d=std::max(this->D,other.D)/std::min(this->D,other.D);
	BRYVector new_bry;
	if(this->D<=other.D){
		__BRYAdd1(B); __BRYAdd1(R); __BRYAdd1(Y); __BRYAdd1(D);
	}else{
		__BRYAdd2(B); __BRYAdd2(R); __BRYAdd2(Y); __BRYAdd2(D);
	}
	return new_bry;
}
BRYVector BRYVector::operator*(int mult) const{
	BRYVector new_bry(*this);
	new_bry.B*=mult; new_bry.R*=mult; new_bry.Y*=mult; new_bry.D*=mult;
	return new_bry;
}
#define __BRYAdd3(member) this->member=(this->member*multiply_d)+other.member
#define __BRYAdd4(member) this->member=(other.member*multiply_d)+this->member
BRYVector& BRYVector::operator+=(const BRYVector& other){
	int multiply_d=std::max(this->D,other.D)/std::min(this->D,other.D);
	if(this->D<=other.D){
		__BRYAdd3(B); __BRYAdd3(R); __BRYAdd3(Y); __BRYAdd3(D);
	}else{
		__BRYAdd4(B); __BRYAdd4(R); __BRYAdd4(Y); __BRYAdd4(D);
	}
	simplify();
	return *this;
}
bool BRYVector::operator==(const BRYVector& other) const{
	if(memcmp(this,&other,sizeof(BRYVector))==0) return true; //Compare if all member values are the same by memcmp(,,sizeof(BRYVector))==0.
	const int multiply_d=std::max(this->D,other.D)/std::min(this->D,other.D); //Compare B/R/Y/D like fractions where D is the denominator.
	const BRYVector bry_cmp(((this->D<=other.D)?*this:other)*multiply_d);
	return memcmp(&bry_cmp,((this->D<=other.D)?&other:this),sizeof(BRYVector))==0;
}
void BRYVector::print() const{
	printf("B=%d,R=%d,Y=%d,D=%d\n",this->B,this->R,this->Y,this->D);
}
#define __AsULL(u_sh) static_cast<unsigned long long>(u_sh)
unsigned long long BRYVector::to_hash() const{
	return (__AsULL(B)<<48)|(__AsULL(R)<<32)|(__AsULL(Y)<<16)|(__AsULL(D));
}
void BRYVector::simplify(){//"Divide" by multiples of 2 if all has 1-bit (or is all odd).
	while(!(B&1)&&!(R&1)&&!(Y&1)&&!(D&1)){B>>=1;R>>=1;Y>>=1;D>>=1;}
}
//Requires manual deletion.
void BRYVector::get_fractions(std::string& B_s,std::string& R_s,std::string& Y_s) const{
	B_s=std::to_string(B)+"/"+std::to_string(D)+" B";
	R_s=std::to_string(R)+"/"+std::to_string(D)+" R";
	Y_s=std::to_string(Y)+"/"+std::to_string(D)+" Y";
}
BRYVector rpn_to_color(const std::span<char>& rpn_c,const std::span<char>& char_span,const std::span<BRYVector>& char_color_span){
	BRYVector* bry_stack=new BRYVector[(rpn_c.size()/2)+1]();
	std::size_t stack_i=0,char_i=-1,rpn_i=0;
	for(;rpn_i!=rpn_c.size();rpn_i++){
		char_i=-1;
		char ch=rpn_c[rpn_i];
		if(ch!='+'){//Non '+', put index BRYVector by char in stack.
			while(char_span[++char_i]!=ch){}
			bry_stack[stack_i++]=char_color_span[char_i];
			continue;
		}//For '+' operator, "Merge last two elements from stack by midpoints"
		bry_stack[stack_i-2]+=bry_stack[stack_i-1];
		stack_i--;
	}
	BRYVector return_this=bry_stack[stack_i-1];//Last BRYVector in stack.
	delete[] bry_stack;
	return return_this;
}
BRYVector rpn_to_bryvector(const char* rpn_c){
	const int rpn_c_size=strlen(rpn_c);
	BRYVector* bry_stack=new BRYVector[(rpn_c_size/2)+1]();
	int stack_i=0,rpn_i=0;
	for(;rpn_i!=rpn_c_size;rpn_i++){
		char ch=rpn_c[rpn_i];
		if(ch!='+'){//Non '+', put index BRYVector by valid char in stack.
			bry_stack[stack_i++]=BRYVector(ch);
			continue;
		}
		if(stack_i-2<0) throw std::logic_error("Too many '+' operators.");
		bry_stack[stack_i-2]+=bry_stack[stack_i-1];//For '+' operator, "Merge last two elements from stack"
		stack_i--;
	}
	if(stack_i-1!=0) throw std::logic_error("Too few '+' operators.");
	BRYVector return_this=bry_stack[stack_i-1];//Last BRYVector in stack.
	delete[] bry_stack;
	return return_this;
}
ColorMidpointInfix rpn_to_infix(const char* rpn_c){
	const int rpn_c_size=strlen(rpn_c);
	ColorMidpointInfix* cmi_stack(new ColorMidpointInfix[(rpn_c_size/2)+1]());//Max floor(length/2)+1 operands
	int value_i=0,stack_i=0;
	while(value_i!=rpn_c_size){
		if(rpn_c[value_i]!='+'){//Insert single char ColorMidpointInfix objs onto stack.
			cmi_stack[stack_i++]=ColorMidpointInfix(rpn_c[value_i++]);
			continue;
		}//For '+' operator, "Pop 2" ColorMidpointInfix objs, "Insert 1 merged" back.
		if(stack_i-2<0) throw std::logic_error("Too many '+' operators.");
		cmi_stack[stack_i-2]=cmi_stack[stack_i-1]+cmi_stack[stack_i-2];
		stack_i--;
		value_i++;
	}
	if(stack_i-1!=0) throw std::logic_error("Too few '+' operators.");
	ColorMidpointInfix return_this=cmi_stack[stack_i-1];//Copy the only merged object in the stack.
	delete[] cmi_stack;
	return return_this;
}
#define __FRAC(num1,num2) (static_cast<float>(num1)/static_cast<float>(num2))
#define __SQU(exp) ((exp)*(exp))
#define __PYTHAGOR_SQR(x1,y1,z1,x2,y2,z2) __SQU(x1-x2)+__SQU(y1-y2)+__SQU(z1-z2)
#define __AREAL_RATIO(s1,s2,s3) (2.f*s1*s2+2.f*s1*s3+2.f*s2*s3-__SQU(s1)-__SQU(s2)-__SQU(s3))/(2.f*ab*bc+2.f*ab*ac+2.f*bc*ab-__SQU(ab)-__SQU(bc)-__SQU(ac))
/*Input cartesian triangle coordinates A(x1,y1,z1), B(x2,y2,z3), C(x3,y3,z3), assuming point P(x4,y4,z4) is coplanar and within the points. Output areal coordinates which is
the ratio of Triangle Areas PBC, PAC, and PAB divided by ABC, where the sum of the ratios equal to 1.*/
std::tuple<float,float,float> get_areal_coordinates(float x1, float y1, float z1, float x2, float y2, float z2,
	float x3, float y3, float z3, float x4, float y4, float z4){
	const auto [ap,bp,cp,bc,ac,ab]=std::tuple(__PYTHAGOR_SQR(x1,y1,z1,x4,y4,z4) //Constants are actually squared pythagorean.
		,__PYTHAGOR_SQR(x2,y2,z2,x4,y4,z4)
		,__PYTHAGOR_SQR(x3,y3,z3,x4,y4,z4)
		,__PYTHAGOR_SQR(x2,y2,z2,x3,y3,z3)
		,__PYTHAGOR_SQR(x1,y1,z1,x3,y3,z3)
		,__PYTHAGOR_SQR(x1,y1,z1,x2,y2,z2));
	auto [p1,p2,p3]=std::tuple(sqrtf(__AREAL_RATIO(bc,cp,bp)),sqrtf(__AREAL_RATIO(ac,cp,ap)),sqrtf(__AREAL_RATIO(ab,bp,ap)));//__AREAL_RATIO is 2 Heron's Triangle Area formula with (1.f/4.f) cancelled, variables expanded, and sqrtf only used once.
	p1=std::isnan(p1)?0.f:p1; p2=std::isnan(p2)?0.f:p2; p3=std::isnan(p3)?0.f:p3; //NaN checks.
	return std::tuple(p1,p2,p3);
}
constexpr void swap_index_if_lesser(Colors* rank,int* color_num,std::size_t i1,std::size_t i2){
	if(color_num[i1]<color_num[i2]){
		std::swap(color_num[i1],color_num[i2]);
		std::swap(rank[i1],rank[i2]);
	}
}
//Maps to scalar=0,color=c1 to scalar=scalar_max/2,color=c2 to scalar=scalar_max,color=c3
//Used for 3 color interpolation such as interpolating from blue to green to yellow rather than blue to gray to yellow by linear interpolation.
constexpr int three_color_interp(float c1,float c2,float c3,float scalar,float scalar_max){
	return (scalar<=(scalar_max/2.f)) ? static_cast<int>(c1+(c2-c1)*(scalar*2.f/scalar_max))
	: static_cast<int>(c2+(c3-c2)*(2.f*scalar/scalar_max-1.f));
}
unsigned int BRYColorWheel::get_color(unsigned char alpha) const{
	Colors rank[3]={Colors::B,Colors::R,Colors::Y};//Used to rank from greatest [0] to least [2].
	int color_num[3]={bryv.B,bryv.R,bryv.Y};
	swap_index_if_lesser(rank,color_num,0,1);
	swap_index_if_lesser(rank,color_num,0,2);
	swap_index_if_lesser(rank,color_num,1,2);
	//Code below separates into 3 sub-triangles that share the middle point (1/3,1/3,1/3). The least color compared is excluded as that point for the triangles.
	if(rank[2]==Colors::B){
		auto [gr_scl,r_scl,y_scl]=get_areal_coordinates(1.f/3.f,1.f/3.f,1.f/3.f,0,1,0,0,0,1,__FRAC(bryv.B,bryv.D),__FRAC(bryv.R,bryv.D),__FRAC(bryv.Y,bryv.D));
		return static_cast<int>(gr_scl*CEnums::gr[0]+r_scl*CEnums::r[0]+y_scl*CEnums::y[0])<<24
		|static_cast<int>(gr_scl*CEnums::gr[1]+r_scl*CEnums::r[1]+y_scl*CEnums::y[1])<<16
		|static_cast<int>(gr_scl*CEnums::gr[2]+r_scl*CEnums::r[2]+y_scl*CEnums::y[2])<<8
		|static_cast<int>(alpha);
	}else if(rank[2]==Colors::R){
		auto [gr_scl,b_scl,y_scl]=get_areal_coordinates(1.f/3.f,1.f/3.f,1.f/3.f,1,0,0,0,0,1,__FRAC(bryv.B,bryv.D),__FRAC(bryv.R,bryv.D),__FRAC(bryv.Y,bryv.D));
		return static_cast<int>(gr_scl*CEnums::gr[0]+(1.f-gr_scl)*three_color_interp(CEnums::b[0],CEnums::g[0],CEnums::y[0],y_scl,b_scl+y_scl))<<24
		|static_cast<int>(gr_scl*CEnums::gr[1]+(1.f-gr_scl)*three_color_interp(CEnums::b[1],CEnums::g[1],CEnums::y[1],y_scl,b_scl+y_scl))<<16
		|static_cast<int>(gr_scl*CEnums::gr[2]+(1.f-gr_scl)*three_color_interp(CEnums::b[2],CEnums::g[2],CEnums::y[2],y_scl,b_scl+y_scl))<<8
		|static_cast<int>(alpha);
	}else{
		auto [gr_scl,b_scl,r_scl]=get_areal_coordinates(1.f/3.f,1.f/3.f,1.f/3.f,1,0,0,0,1,0,__FRAC(bryv.B,bryv.D),__FRAC(bryv.R,bryv.D),__FRAC(bryv.Y,bryv.D));
		return static_cast<int>(gr_scl*CEnums::gr[0]+(1.f-gr_scl)*three_color_interp(CEnums::b[0],CEnums::m[0],CEnums::r[0],r_scl,b_scl+r_scl))<<24
		|static_cast<int>(gr_scl*CEnums::gr[1]+(1.f-gr_scl)*three_color_interp(CEnums::b[1],CEnums::m[1],CEnums::r[1],r_scl,b_scl+r_scl))<<16
		|static_cast<int>(gr_scl*CEnums::gr[2]+(1.f-gr_scl)*three_color_interp(CEnums::b[2],CEnums::m[2],CEnums::r[2],r_scl,b_scl+r_scl))<<8
		|static_cast<int>(alpha);
	}
}