#include "generators.hpp"
DyckWordsGen::DyckWordsGen():bits(),bits_size(0),catalan_n(0){}
/*num_one_bits = Number of 1-bits in 2x string and for catalan number. Should be >0*/
DyckWordsGen::DyckWordsGen(std::size_t num_one_bits)
	:bits(new bool[num_one_bits*2]()),bits_size(num_one_bits*2),catalan_n(get_catalan(num_one_bits)){
	assert(num_one_bits>0);
	for(std::size_t i=0;i!=bits_size;i++){
		bits[i]=(i/num_one_bits);//Fill left-half with 0's and right-half 1's
	}
}
DyckWordsGen::~DyckWordsGen(){
	delete[] bits;
}
void DyckWordsGen::print_word(){
	for(std::size_t bits_i=0;bits_i!=bits_size;bits_i++){
		printf("%d",bits[bits_i]);
	}
	printf("\n");
}
std::size_t DyckWordsGen::get_max_permutations(){
	return catalan_n;
}

/*Returns next dyck word.
[[nodiscard]]: Return value requires delete[] span.data().*/
[[nodiscard]] std::span<bool> DyckWordsGen::next(){
	std::span<bool>bool_table{new bool[bits_size](),bits_size};
	memcpy(bool_table.data(),bits,bits_size*sizeof(bool));
	next_dyck_word();
	return bool_table;
}
/*Returns next dyck word using span of chars.
Required: (Dyck word length)/2+1==(chars length)
[[nodiscard]]: Return value requires delete[] span.data().*/
[[nodiscard]] std::span<char> DyckWordsGen::next_rpn(const MultiSetStringGen& mss_gen){
	std::span<char>return_this{new char[bits_size+1](),bits_size+1};//(# chars in multiset)+(# 1-bits)
	return_this[0]=mss_gen.value[0];//First char is always an operand, don't compare to dyck word yet.
	std::size_t bit_i=0,ret_i=1,char_i=1;
	while(bit_i!=bits_size){
		return_this[ret_i++]=bits[bit_i++]?'+':mss_gen.value[char_i++];
	}//^'+' if true. Get next char in multiset otherwise.
	next_dyck_word();
	return return_this;
}
/*(# of chars in MultiSet) should be 1 more than
(# of 1-bits in Dyck Word)/2 to do RPN correctly*/
void DyckWordsGen::verify_multiset_rpn(const MultiSetStringGen& mss_gen){
	assert(mss_gen.value_size==bits_size/2+1);
}
/*Next dyck word in lexicographical order (0 then 1).
If n=4, First: 00001111. Next: 00010111. Next: 00011011...
Last would be alternating 0's then 1's. (01010101)
Using catalan number in for loop would print all of the words.*/
inline void DyckWordsGen::next_dyck_word(){
	std::size_t bit_shift_max=2;//Each bit from right to left can move left +1 more, then swap back to initial position.
	std::size_t bit_pos_now=1;
	std::size_t bit_i=bits_size-1;//Disregarding rightmost bit in while loop (bit_shift_max:=1).
	while(bit_i--!=0){
		/*printf("Reading bit %d(=%d) and bit %d(=%d). Max bit shift %d. Bit position now %d.\n"
		,bit_i,bits[bit_i]
		,bit_i-1,bits[bit_i-1],
		bit_shift_max,bit_pos_now);*/
		if(!bits[bit_i]){
			if(bits[bit_i-1]){//(10), swap 1-bit back to initial position.
				bits[--bit_i]=0; bits[bit_i+(bit_shift_max++)-1]=1;
			}
			bit_pos_now++;//(00 and 10)
			continue;
		}
		if(bit_pos_now!=bit_shift_max){//Is free to left shift.
			if(bits[bit_i-1]){//(11), swap 1-bit back to initial position.
				bits[bit_i]=0; bits[bit_i+bit_pos_now-1]=1;
				bit_shift_max++;
				continue;
			}//(01), shift left and finish algorithm (Is next dyck word).
			bits[bit_i]=0; bits[bit_i-1]=1;
			return;
		}//Left shift at max. Swap 1-bit back to initial position regardless of index:=bit_i-1.
		bits[bit_i]=0; bits[bit_shift_max-1]=1;
		bit_shift_max++;
	}
}

MultiSetStringGen::MultiSetStringGen():value(),value_size(0),char_set(),char_set_size(0),max_permutations(0){}
MultiSetStringGen::MultiSetStringGen(std::size_t length,const std::span<char> char_set_copy)
:value(new char[length]())
,value_size(length)
,char_set(new char[char_set_copy.size()]())
,char_set_size(char_set_copy.size())//To copy values of char_set.
,max_permutations(std::pow(char_set_copy.size(),length)){
	memcpy(this->char_set,char_set_copy.data(),char_set_size*sizeof(char));
	for(std::size_t value_i=0;value_i!=value_size;value_i++){
		value[value_i]=char_set[0];//Initialize all as first character in char_set.
	}
}
MultiSetStringGen::~MultiSetStringGen(){
	delete[] value;
	delete[] char_set;
}
void MultiSetStringGen::print_chars(){
	for(std::size_t value_i=0;value_i!=value_size;value++){printf("%c",value[value_i]);}
	printf("\n");
}
std::size_t MultiSetStringGen::get_max_permutations(){
	return max_permutations;
}
/*Increments the next character in a char set span from right to left.
Ex: If char set is {'x','y','z'},
and the current characters are "yyzx", then this returns "yyzy".
If current is "xxz", then the next is "xyx".*/
void MultiSetStringGen::next(){
	std::size_t next_char_i=-1;//-1 to include index 0.
	//Increments, from right to left, the characters from char_set.
	for(int value_i=value_size-1;value_i>-1;value_i--){
		while(char_set[next_char_i++]!=value[value_i]){}
		if(next_char_i!=char_set_size){
			value[value_i]=char_set[next_char_i];//Increment to next character.
			break;
		}
		value[value_i]=char_set[0];//"Carry the one" and increment next character in for loop.
		next_char_i=-1;//Reset for next character.
	}
}
/* When using DyckWordsGen::next() for iteration */
std::size_t get_catalan(std::size_t n){
	if (n<=1) return 1;
	std::size_t res = 0; 
	for (std::size_t i=0; i<n; i++)
		res += get_catalan(i)*get_catalan(n-i-1);
	return res;
}
/*Returns bool if string ColorMidpointRPN is not shortened/simplified and the resulting string.*/
std::pair<bool,ColorMidpointRPN> verify_unmerged_rpn(const std::span<char>& ms){
	RPNStack<ColorMidpointRPN> cmr_stack(ms.size());
	ColorMidpointRPN return_this;
	bool is_unmergeable=true;
	for(auto const& ms_ch:ms){
		if(ms_ch!='+'){//Non '+', put in stack.
			cmr_stack.push(ColorMidpointRPN(ms_ch));
			continue;
		}//For '+' operator, "Pop 2" ColorMidpointRPN, "Insert 1 merged" back.
		auto[v1,v2]=cmr_stack.pop_two();
		return_this=v1+v2;
		if(return_this.length==v1.length+v2.length+1){//If it did not simplify. +1 for '+'.
			cmr_stack.push(return_this);//Add merged result.
			continue;
		}
		is_unmergeable=false;
		return_this=ColorMidpointRPN();//Default constructor to clear.
		break;
	}
	return std::pair<bool,ColorMidpointRPN>{is_unmergeable,return_this};
}