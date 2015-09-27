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

#ifndef RESULT_TYPES_H
#define RESULT_TYPES_H

#include <vector>
#include <string>
#include "parsers.h"

//! @file this header defines some useful custom result types

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

//! result type for parsing only, no phrasing
struct parse
{
	//! result of child of type T
	template<class T>
	static parse mk_result(const T& ) { return parse(); }

	template<class Itr1, class Itr2>
	parse(const Itr1&, const Itr2& ) {}
	parse() {}

	void append(const parse& ) const {}
	friend std::ostream& operator<<(std::ostream& o, parse& p);
};

inline std::ostream& operator<<(std::ostream& o, parse& ) { return o; }

#endif // RESULT_TYPES_H
