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

#include <vector>
#include <cstdlib>

#include "c_tokens.h"
#include "result_types.h"
#include "c_grammar.h"

class result_test : public
	regex< regex< raw<'a'>, raw<'c'> >, raw<'b'> > {};

std::size_t n_matches(const char* s1, const char* s2) {
	std::size_t res = 0;
	for(; (*s1) && (*s2) && (*s1 == *s2); ++s1, ++s2, ++res) std::cerr << +*s1 << ", " << +*s2 << std::endl;
	std::cerr << +*s1 << ", " << +*s2 << std::endl;
	return res;
}

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

	//! required for terminals
	template<class Itr1, class Itr2>
	token_vector(const Itr1&, const Itr2& ) {}
	//! required by the parser
	token_vector() {}

	void clear() { v.clear(); }

	void append(const token_type& tok) const {
		v.push_back((char)tok);
	}

	void append(const token_vector& ) const {}

	friend std::ostream& operator<<(std::ostream& o, token_vector& tv);
};

// static members
std::vector<char> token_vector::v;

inline std::ostream& operator<<(std::ostream& o, token_vector& tv)
{
	for(std::vector<char>::const_iterator itr = tv.v.begin();
		itr != tv.v.end(); ++itr)
			o << +*itr << std::endl;
	return o;
}

template<class Regex, class Result, class CmpRes>
void assert_match(const char* str, const CmpRes& exp_res, std::size_t digits) {
	std::string s = str;
	std::string::const_iterator itr0 = s.begin(), itr = s.begin();

	//typedef typename mk_action<debugger, Regex>::type ac_regex;
	typedef Regex ac_regex;
	Result res;
	if(! ac_regex::match(itr, res) )
	 std::cerr << "WARNING: did not match..." << std::endl;

	std::cout << "Parsing result: " << res << std::endl;



	if((std::size_t)std::distance(itr0, itr) != digits)
	{
		std::cerr << "signs parsed: " << std::distance(itr0, itr) << " of " << digits << std::endl;
		std::cerr << "at: " << (&*itr) << std::endl;
		 throw "regex test error";
	}
}

template<class Regex>
constexpr bool c_assert_match(const char* str, std::size_t digits,
	const char* itr0 = NULL,
	const char* itr = NULL) {

	typedef typename mk_action<debugger, Regex>::type ac_regex;
	/*static_assert((
		itr0 = str,
		itr = str,
		ac_regex::match(itr),
		std::distance(itr0, itr) == digits),
		"static match failure");*/
	/*return itr0 = str,
		itr = str,
		ac_regex::match(itr),
		std::distance(itr0, itr) == digits;*/
		return -(std::size_t)(str - ac_regex::match(str)) == digits;
}

template<class Regex, class Result, class CmpRes>
void assert_match(const char* str, const CmpRes& exp_res = CmpRes()) {
	assert_match<Regex, Result>(str, exp_res, strlen(str));
}

template<class Regex, class Result>
void assert_match(const char* str) {
	assert_match<Regex, Result, Result>(str);
}

template<class Regex>
constexpr bool c_assert_match(const char* str) {
	return c_assert_match<Regex>(str, strlen(str));
}

void assert_match_ctoken(const char* input, const char* exp_res)
{
	token_vector tv;
	assert_match<c99_file, token_vector>(input, tv);
	std::size_t matches;
	if( strlen(exp_res) != (matches = n_matches(exp_res, tv.v.data()))) {
		std::cerr << "only matched: " << matches << std::endl;
		throw "matching error";
	}

	tv.clear();
}

class r2;

class r1 : public
	choices< raw<'a'>, regex<raw<'b'>, r2 > > {};

class r2 : public
	choices<r1, raw<'c'> > {};

inline void test_regex()
{
	assert_match<hex_const, parse>("0xA1");
	assert_match<hex_const, parse>("0xA1ul");

	assert_match<r_string_literal, parse>("L\"hello\""); // TODO: \" ?

	assert_match<r_float_const_2, parse>(".52e+f");
	assert_match<r_float_const_2, parse>(".52f");

	const char exp_res[41] = {
		k_int, whitespace, identifier, lpar, rpar, whitespace, lbrace, whitespace,
		k_int, whitespace, identifier, whitespace, eq_sgn, whitespace, constant, semicolon,
		k_int, whitespace, identifier, whitespace, eq_sgn, whitespace, inc_op, identifier, whitespace,
			percent, whitespace, identifier, whitespace, circ, whitespace, constant, semicolon,
		k_return, whitespace, constant, semicolon, whitespace, rbrace, whitespace, 0
	};
	assert_match_ctoken("int main() {\n"
		"int x = 0;"
		"int y = ++x % x ^ 1;"
		"return 0; }\n", exp_res);

	//assert_match<translation_unit, parse>(exp_res);
	assert_match<r1, parse>("bba");

	const char* str = "acb";
	string_res res;
	result_test::match(str, res);
	std::cerr << "my result: " << res.res << std::endl;
}

void print_err(const char* msg) {
	std::cerr << "Aborting on error caught: " << std::endl
		<< msg << "." << std::endl;
}

int main()
{
	int exit_code = EXIT_SUCCESS;
	try
	{
		test_regex();
	} catch (std::string msg) {
		print_err(msg.c_str());
		exit_code = EXIT_FAILURE;
	} catch (const char* msg) {
		print_err(msg);
		exit_code = EXIT_FAILURE;
	}
	std::cerr << ((exit_code == EXIT_SUCCESS)
			? "Success!"
			: "Failure!")
			<< std::endl;
	return exit_code;
}

