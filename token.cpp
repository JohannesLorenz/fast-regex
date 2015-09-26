#include <algorithm>
#include <iostream>
#include "token.h"
#include "c_grammar.h"

// static members
std::vector<char> token_vector::v;

/*token_t::token_t()
{
}*/

std::ostream& token_t::dump(std::ostream& stream, const std::string& file) {
	std::string substr;
	std::copy(_begin, _end, std::back_inserter(substr));
	return stream << substr << *this;
	//return stream << file.substr(begin, end) << *this;
}

//#define K_TOKEN(x) add("x", k_##x
#define K_TOKEN(x) add(#x, k_##x);

void token_map::init_tokens()
{
	//add(identifier, "<identifier>";
	//add(k_auto, "auto";
	//add(k_break, "break";
	//add(k_case,
	K_TOKEN(auto);
	K_TOKEN(break);
	K_TOKEN(case);
	K_TOKEN(char);
	K_TOKEN(const);
	K_TOKEN(continue);
	K_TOKEN(default);
	K_TOKEN(do);
	K_TOKEN(double);
	K_TOKEN(else);
	K_TOKEN(enum);
	K_TOKEN(extern);
	K_TOKEN(float);
	K_TOKEN(for);
	K_TOKEN(goto);
	K_TOKEN(if);
	K_TOKEN(int);
	K_TOKEN(long);
	K_TOKEN(register);
	K_TOKEN(return);
	K_TOKEN(short);
	K_TOKEN(signed);
	K_TOKEN(sizeof);
	K_TOKEN(static);
	K_TOKEN(struct);
	K_TOKEN(switch);
	K_TOKEN(typedef);
	K_TOKEN(union);
	K_TOKEN(unsigned);
	K_TOKEN(void);
	K_TOKEN(volatile);
	K_TOKEN(while);
	
	add("...", ellipsis);
	add(">>=", right_assign);
	add("<<=", left_assign);
	add("+=", add_assign);
	add("-=", sub_assign);
	add("*=", mul_assign);
	add("/=", div_assign);
	add("%=", mod_assign);
	add("&=", and_assign);
	add("^=", xor_assign);
	add("|=", or_assign);
	add(">>", right_op);
	add("<<", left_op);
	add("++", inc_op);

	add(" ", k_do); // TODO!!
	//tokens.push_back(new hex_const());
}

#undef K_TOKEN

token_map::token_map()
{
	init_tokens();
}

cfile::cfile(const std::string& content)
{
	std::string::const_iterator itr = content.begin();
	token_t cur_token = token_reader::invalid(itr);
	if(itr != content.end())
	do
	{
		cur_token = map.apply(content, itr);
		_tokens.push_back(cur_token);
		std::cerr << cur_token.dump(std::cerr, content);
		std::cerr << std::endl;
	}
	while(itr != content.end() && cur_token);

	if(! cur_token)
	 throw std::string("invalid token found: ") + &*itr;
}

