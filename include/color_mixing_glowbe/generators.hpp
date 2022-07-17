#ifndef _DYCK_WORDS_HPP_
#define _DYCK_WORDS_HPP_
#include <cstdio>
#include <cmath>
#include <span>
#include "color_mixing.hpp"
#include "rpn_stack.hpp"
class MultiSetStringGen;
class ColorMidpointRPN;
class DyckWordsGen{
public:
	DyckWordsGen();
	DyckWordsGen(size_t num_one_bits);
	~DyckWordsGen();
	void print_word();
	size_t get_max_permutations();
	[[nodiscard("Memory leak. Requires delete[].")]] std::span<bool> next();
	[[nodiscard("Memory leak. Requires delete[].")]] std::span<char> next_rpn(const MultiSetStringGen& mss_gen);
	void verify_multiset_rpn(const MultiSetStringGen& mss_gen);
private:
	bool* bits;
	const std::size_t bits_size;
	const std::size_t catalan_n;
	void next_dyck_word();
};
/*Generator to create a mathematical #length multiset from a set of span<char>*/
class MultiSetStringGen{
public:
	MultiSetStringGen();
	MultiSetStringGen(size_t length,const std::span<char> char_set);
	~MultiSetStringGen();
	void print_chars();
	size_t get_max_permutations();
	void next();
	friend DyckWordsGen;
private:
	char* value;
	const std::size_t value_size;
	char* char_set;
	const std::size_t char_set_size;
	const std::size_t max_permutations;
};
size_t get_catalan(size_t n);
std::pair<bool,ColorMidpointRPN> verify_unmerged_rpn(const std::span<char>& ms);
#endif