/*************************************************************************/
/* cparser - a C++03, template based parser                              */
/* Copyright (C) 2015-2015                                               */
/* Johannes Lorenz (jlsf2013 @ sourceforge)                              */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#ifndef C_TOKENS_H
#define C_TOKENS_H

#include "parsers.h"

enum token_type
{
	no_token, // an invalid token, like \0 in strings

	identifier, constant, string_literal, k_sizeof, // 1...
	ptr_op, inc_op, dec_op, left_op, right_op, le_op, ge_op, eq_op, ne_op, // 5
	and_op, or_op, mul_assign, div_assign, mod_assign, add_assign, // 14
	sub_assign, left_assign, right_assign, and_assign, // 20
	xor_assign, or_assign, type_name, // 24

	k_typedef, k_extern, k_static, k_auto, k_register, // 27
	k_char, k_short, k_int, k_long, k_signed, k_unsigned, // 32
	k_float, k_double, k_const, k_volatile, k_void, // 38
	k_struct, k_union, k_enum, ellipsis, // 43

	k_case, k_default, k_if, k_else, k_switch, // 47
	k_while, k_do, k_for, k_goto, k_continue, k_break, k_return, // 52

	// "simple signs":
	// braces: {}, parantheses: (), brackets: [], angles: <>
	semicolon, lbrace, // 59
	rbrace, comma, colon, eq_sgn, lpar, rpar, lbrack, rbrack, // 61
	point, and_sgn, excl_mark, tilde, minus_sgn, plus_sgn, star, slash, // 69
	percent, langle, rangle, circ, vbar, qu_mark, whitespace, // 77

	none // 84 // TODO: deprecated?
};

typedef regex<	raw<'0'>,
		one_of<'x', 'X'>,
		kleene_p<r_isxdigit>,
		kleene<r_isintsgn>
	> hex_const;

typedef regex<	raw<'0'>,
		kleene_p<r_isdigit>,
		kleene<r_isintsgn>
	> r_oct_const;

typedef regex<	kleene_p<r_isdigit>,
		kleene<r_isintsgn>
	> r_dec_const;

typedef regex<	maybe< raw<'L'> >,
		raw<'\''>,
		kleene_p< r_isnot<'\''> >,
		raw<'\''>
	> r_char_const;

typedef regex<	one_of<'e', 'E'>,
		maybe< one_of<'+', '-'> >
	> r_exp_part;

class r_identifier : public
	regex<	r_iscletter1,
		kleene<r_iscletter>
	> {};

typedef one_of<'f', 'F', 'l', 'L'> r_floatsgn;

class r_float_const : public
	regex<
		kleene_p<r_isdigit>,
		r_exp_part,
		maybe<r_floatsgn>
	> {};

class r_float_const_2 :
	public regex<
		kleene<r_isdigit>,
		raw<'.'>,
		kleene_p<r_isdigit>,
		maybe<r_exp_part>,
		maybe<r_floatsgn>
	> {};

class r_float_const_3 : public
	regex<
		kleene_p<r_isdigit>,
		raw<'.'>,
		kleene<r_isdigit>,
		maybe<r_exp_part>,
		maybe<r_floatsgn>
	> {};

class r_string_literal : public
	regex<	maybe< raw<'L'> >,
		raw<'"'>,
		kleene_p< r_isnot<'"'> >,
		raw<'"'>
	> {};

template<token_type Token> class has_token {};

template<token_type Token, char C1, char C2 = 0, char C3 = 0, char C4 = 0,
	char C5 = 0, char C6 = 0, char C7 = 0, char C8 = 0>
class tok_str : public has_token<Token>, public str<C1, C2, C3, C4, C5, C6, C7, C8> {}; // TODO: reuse tok_regex?

template<token_type Token, class C1, class C2 = Nothing,
	class C3 = Nothing, class C4 = Nothing,
	class C5 = Nothing, class C6 = Nothing,
	class C7 = Nothing, class C8 = Nothing>
class tok_regex : public has_token<Token>, public regex<C1, C2, C3, C4, C5, C6, C7, C8> {};

class c_keywords_1 : public
	choices <
		tok_str<k_auto, 'a', 'u', 't', 'o'>,
		tok_str<k_break, 'b', 'r', 'e', 'a', 'k'>,
		tok_str<k_case, 'c', 'a', 's', 'e'>,
		tok_str<k_char, 'c', 'h', 'a', 'r'>,
		tok_str<k_const, 'c', 'o', 'n', 's', 't'>,
		tok_str<k_continue, 'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>,
		tok_str<k_default, 'd', 'e', 'f', 'a', 'u', 'l', 't'>,
		tok_str<k_do, 'd', 'o'>
	> {};


class c_keywords_2 : public
	choices<
		tok_str<k_double, 'd', 'o', 'u', 'b', 'l', 'e'>,
		tok_str<k_else, 'e', 'l', 's', 'e'>,
		tok_str<k_enum, 'e', 'n', 'u', 'm'>,
		tok_str<k_extern, 'e', 'x', 't', 'e', 'r', 'n'>,
		tok_str<k_float, 'f', 'l', 'o', 'a', 't'>,
		tok_str<k_for, 'f', 'o', 'r'>,
		tok_str<k_goto, 'g', 'o', 't', 'o'>,
		tok_str<k_if, 'i', 'f'>
	> {};

class c_keywords_3 : public
	choices<
		tok_str<k_int, 'i', 'n', 't'>,
		tok_str<k_long, 'l', 'o', 'n', 'g'>,
		tok_str<k_register, 'r', 'e', 'g', 'i', 's', 't', 'e', 'r'>,
		tok_str<k_return, 'r', 'e', 't', 'u', 'r', 'n'>,
		tok_str<k_short, 's', 'h', 'o', 'r', 't'>,
		tok_str<k_signed, 's', 'i', 'g', 'n', 'e', 'd'>,
		tok_str<k_sizeof, 's', 'i', 'z', 'e', 'o', 'f'>,
		tok_str<k_static, 's', 't', 'a', 't', 'i', 'c'>
	> {};

class c_keywords_4 : public
	choices<
		tok_str<k_struct, 's', 't', 'r', 'u', 'c', 't'>,
		tok_str<k_switch, 's', 'w', 'i', 't', 'c', 'h'>,
		tok_str<k_typedef, 't', 'y', 'p', 'e', 'd', 'e', 'f'>,
		tok_str<k_union, 'u', 'n', 'i', 'o', 'n'>,
		tok_str<k_unsigned, 'u', 'n', 's', 'i', 'g', 'n', 'e', 'd'>,
		tok_str<k_void, 'v', 'o', 'i', 'd'>,
		tok_str<k_volatile, 'v', 'o', 'l', 'a', 't', 'i', 'l', 'e'>,
		tok_str<k_while, 'w', 'h', 'i', 'l', 'e'>
	> {};

class c_keywords : public
	choices<
		c_keywords_1,
		c_keywords_2,
		c_keywords_3,
		c_keywords_4
	> {};

class c_constants : public
	choices<
		tok_regex<identifier, r_identifier>, // TODO: this is no constant
		tok_regex<constant, choices<
			hex_const,
			r_oct_const,
			r_dec_const,
			r_char_const,
			r_float_const,
			r_float_const_2,
			r_float_const_3>
			>,
		tok_regex<string_literal, r_string_literal>
	> {};

class c_other_1 : public
	choices<
		tok_str<ellipsis, '.', '.', '.'>,
		tok_str<right_assign, '>', '>', '='>,
		tok_str<left_assign, '<', '<', '"'>,
		tok_str<add_assign, '+', '='>,
		tok_str<sub_assign, '-', '='>,
		tok_str<mul_assign, '*', '='>,
		tok_str<div_assign, '/', '='>,
		tok_str<mod_assign, '%', '='>
	> {};

class c_other_2 : public
	choices<
		tok_str<and_assign, '&', '='>,
		tok_str<xor_assign, '^', '='>,
		tok_str<or_assign, '|', '='>,
		tok_str<right_op, '>', '>'>,
		tok_str<left_op, '<', '<'>,
		tok_str<inc_op, '+', '+'>,
		tok_str<dec_op, '-', '-'>,
		tok_str<ptr_op, '-', '>'>
	> {};

class c_other_3 : public
	choices<
		tok_str<and_op, '&', '&'>,
		tok_str<or_op, '|', '|'>,
		tok_str<le_op, '<', '='>,
		tok_str<ge_op, '>', '='>,
		tok_str<eq_op, '=', '='>,
		tok_str<ne_op, '!', '='>,
		tok_str<semicolon, ';'>,
		tok_regex<lbrace, choices< raw<'{'> , str<'<', '%'> > >
	> {};

class c_other_4 : public
	choices<
		tok_regex<rbrace, choices< raw<'}'> , str<'%', '>'> > >,
		tok_str<comma, ','>,
		tok_str<colon, ':'>,
		tok_str<eq_sgn, '='>,
		tok_str<lpar, '('>,
		tok_str<rpar, ')'>,
		tok_regex<lbrack, choices< raw<'['> , str<'<', ':'> > >,
		tok_regex<rbrack, choices< raw<']'> , str<':', '>'> > >
	> {};

class c_other_5 : public
	choices<
		tok_str<point, '.'>,
		tok_str<and_sgn, '&'>,
		tok_str<excl_mark, '!'>,
		tok_str<tilde, '~'>,
		tok_str<minus_sgn, '-'>,
		tok_str<plus_sgn, '+'>,
		tok_str<star, '*'>,
		tok_str<slash, '/'>
	> {};

class c_other_6 : public
	choices<
		tok_str<percent, '%'>,
		tok_str<langle, '<'>,
		tok_str<rangle, '>'>,
		tok_str<circ, '^'>,
		tok_str<vbar, '|'>,
		tok_str<qu_mark, '?'>,
		tok_regex<whitespace, one_of<' ', '\t', '\v', '\n', '\f'> >
	> {};

class c_other : public
	choices<
		c_other_1,
		c_other_2,
		c_other_3,
		c_other_4,
		c_other_5,
		c_other_6
	> {};

class c_file_poss : public
	choices<
		c_keywords,
		c_constants,
		c_other
	> {};

class c99_file : public kleene< c_file_poss > {};

class result_test : public
	regex< regex< raw<'a'>, raw<'c'> >, raw<'b'> > {};

//! iterator-pair-constructible char
struct _char
{
	char c;
public:
	operator char() const { return c; }
	_char() {}
	template<class Itr>
	_char(Itr& i1, Itr&) { c = *i1; }
};

#if 0
template<class T>
struct my_result {
	typedef _char type;
}

template<>
struct my_result<result<one_of, raw> >
{

}
#endif

struct string_res
{
	std::string res;
	template<class T> struct result {
		typedef _char type;
	};

	// TODO: sfinae-get return value of function?

	template<char C>
	static _char mk_result(const raw<C>& ) { return _char(); }

	// all other nodes, i.e. containers
	template<class T>
	static string_res mk_result(const T& ) { return string_res(); }


	void append(char c) {
		std::cerr << "app: " << c << std::endl;
		res += c; }

	void append(const string_res& s) {
		std::cerr << "app: " << s.res << std::endl;
		res += s.res; }
};

//! @threadsafe no thread safe (though easily modifiable)
struct token_vector
{
	static std::vector<char> v;

	template<class T>
	static token_vector mk_result(const T& ) { return token_vector(); }

	template<token_type Tok, char C1, char C2, char C3, char C4, char C5, char C6, char C7, char C8>
	static token_type mk_result(const tok_str<Tok, C1, C2, C3, C4, C5, C6, C7, C8>& ) { return Tok; }

	template<token_type Tok, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
	static token_type mk_result(const tok_regex<Tok, C1, C2, C3, C4, C5, C6, C7, C8>& ) { return Tok; }

	template<class Itr1, class Itr2>
	token_vector(const Itr1&, const Itr2& ) {}
	token_vector() {}

	void append(const token_type& tok) const {
		v.push_back((char)tok);
	}

	void append(const token_vector& ) const {}

	friend std::ostream& operator<<(std::ostream& o, token_vector& tv);
};

inline std::ostream& operator<<(std::ostream& o, token_vector& tv)
{
	for(std::vector<char>::const_iterator itr = tv.v.begin();
		itr != tv.v.end(); ++itr)
			o << +*itr << std::endl;
	return o;
}

#if 0
struct tokenizer_res
{
	std::string res;
	/*template<class T> struct result {
		typedef _char type;
	};*/

	template<char C>
	static _char mk_result(const raw<C>& ) { return _char(); }


	void append(char c) {
		std::cerr << "app: " << c << std::endl;
		res += c; }
};
#endif

inline void test_regex()
{
	assert_match<hex_const, parse>("0xA1");
	assert_match<hex_const, parse>("0xA1ul");

	assert_match<r_string_literal, parse>("L\"hello\""); // TODO: \" ?

	assert_match<r_float_const_2, parse>(".52e+f");
	assert_match<r_float_const_2, parse>(".52f");

	assert_match<c99_file, parse>("int main() {\n"
		"int x = 0;"
		"int y = ++x % x ^ 1;"
		"return 0; }\n"
		);

	assert_match<c99_file, token_vector>("int ");

	const char* str = "acb";
	string_res res;
	result_test::match(str, res);
	std::cerr << "my result: " << res.res << std::endl;
}

#endif // C_TOKENS_H
