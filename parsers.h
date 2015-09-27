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

#ifndef TOKEN_H
#define TOKEN_H

//#include <typeinfo>
#include <cstring>
#include <ostream>
//#include <string>
#include <iostream>

#ifdef USE_CPP11
#else
	#define constexpr
#endif

#if 0
template<class token_type>
class token_t
{
	typedef std::string::const_iterator iterator;
	token_type type;
	iterator _begin, _end;
public:
	token_t(token_type type, const iterator& _begin, const iterator& _end) :
		type(type), _begin(_begin), _end(_end)
	{
	}
	
	const iterator& begin() const { return _begin; }
	const iterator& end() const { return _end; }

	//operator bool() const { return type != none; }

	template<class T>
	friend std::ostream& operator<<(std::ostream& o, token_t<T>& t);
	std::ostream& dump(std::ostream& o, const std::string& file);
};

template<class T>
inline std::ostream& operator<<(std::ostream& o, token_t<T>& t) {
	return o << "(token_id:) " << (int)t.type;
}
#endif

template<class T>
class bool_base
{
	typedef bool_base<T> self_t;
public:
	T elem;
	bool val;
	self_t operator&&(self_t& rhs) {
		self_t res;
		return (res.elem = elem && rhs.elem, res.val = val && rhs.val, res);
	}
	self_t operator||(self_t& rhs) {
		self_t res;
		return (res.elem = elem || rhs.elem, res.val = val || rhs.val, res);
	}
};

class parser
{
protected:
	//typedef std::string::const_iterator iterator;
public:
//	typedef bool result;
//	static const bool dont_incr = true;
};

template<bool Result>
struct match_const : public parser
{
	template<class Itr, class R>
	constexpr static bool match(const Itr& , const R& ) {
		return Result;
	}
};

/*
template<char C>
struct raw : public parser {
	typedef raw<C> real_t;
	template<class Itr, class T> // TODO: remove T?
	constexpr static bool match(const Itr& i, const T&) { return *i == C; }
};*/

template<class C> struct basic : public parser
{
	typedef C real_t;
	template<class Itr, class T> // TODO: remove T?
	constexpr static bool match(Itr& i, T& res, Itr start = Itr()) {
		return	start = i,
			(C::ok(i) &&
			(++i, res = T(start, i), true));
	}
	static const bool dont_incr = true;
};

template<char C> struct raw : public basic< raw<C> > {
	template<class Itr>
	constexpr static bool ok(const Itr& i) { return *i == C; }
};

//! represents that nothing more is to be parsed.
struct Nothing : public parser {
	template<class Itr, class T>
	constexpr static bool match(const Itr&, const T&) {
#ifdef USE_CPP11
		return false;
#else
		throw "can not match class \"Nothing\"";
#endif
	}
};

template<class A, class T> struct mk_action;
template<class A> struct mk_action<A, Nothing>;

//! action template for terminals
template<class A, class Terminal> class action : public parser {
public:
	typedef Terminal child_t;
	template<class Itr>
	constexpr static bool match(Itr& i) {
		/*std::cerr << "trying to match terminal:" << std::endl << typeid(Terminal).name() << std::endl;
		bool res = Terminal::match(i);
		if(res)
			std::cerr << " -> success (terminal)!" << std::endl;
		return res;*/
		return Terminal::match(i);
	}
};

template<class A> class action_nt_base : public parser
{
protected:
	template<class Child, class Itr>
	constexpr static bool match_base(Itr& i, typename A::result tmp = typename A::result(), Child* ct = NULL)
	{
		// if the child matched, "collect" it, then reach it up
		// to the next level
		return tmp = Child::match(i) && (A::collect(tmp, ct), true);
	}
};

//! action template for classes with one templ-templ-param
template<class A, template<class > class T, class C1>
class action<A, T<C1> > : public action_nt_base<A> {
public:
	typedef T< typename  mk_action<A, C1>::type > child_t;
	typedef action_nt_base<A> base_t;
	template<class Itr>
	constexpr static bool match(Itr& i) {
	/*	std::cerr << "trying to match:" << std::endl << typeid(T<C1>).name() << std::endl;
		bool res = child_t::match(i);
		if(res)
			std::cerr << " -> success!" << std::endl;
		return res; */
		return base_t::template match_base<child_t>(i);
	}
};

//! action template for classes with 8 templ-templ-param
template<class A,
	template<class, class, class, class, class, class, class, class > class T,
	class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
class action<A, T<C1, C2, C3, C4, C5, C6, C7, C8> > : public action_nt_base<A> {
public:
	typedef T< typename mk_action<A, C1>::type,
		typename mk_action<A, C2>::type,
		typename mk_action<A, C3>::type,
		typename mk_action<A, C4>::type,
		typename mk_action<A, C5>::type,
		typename mk_action<A, C6>::type,
		typename mk_action<A, C7>::type,
		typename mk_action<A, C8>::type
		> child_t;
	typedef action_nt_base<A> base_t;

	template<class Itr>
	constexpr static bool match(Itr& i) {
		// TODO: do not print "Nothing"s?
	/*	std::cerr << "trying to match:" << std::endl << typeid(T<C1, C2, C3, C4, C5, C6, C7, C8>).name() << std::endl;
		bool res = child_t::match(i);
		if(res)
			std::cerr << " -> success!" << std::endl;
		return res;*/
		return base_t::template match_base<child_t>(i);
	}
};

// do not produce action<Nothing> (this would let classes that assume "Nothing" fail
// also, action output for Nothing should be avoided
template<class A, class T> struct mk_action {
	typedef action<A, typename T::real_t> type;
//	typedef mk_action_2<A, T>::type type;	
};
template<class A> struct mk_action<A, Nothing> {
	typedef Nothing type;
};

template<typename T>
class detect_dont_incr
{
	struct fallback { int dont_incr; }; // add member name "dont_incr"
	struct derived : T, fallback { };

	template<typename U, U> struct check;

	typedef char array_of_one[1];
	typedef char array_of_two[2];

	template<typename U>
	static array_of_one & func(check<int fallback::*, &U::dont_incr> *);

	template<typename U>
	static array_of_two & func(...);

public:
	typedef detect_dont_incr type;
	enum { value = sizeof(func<derived>(0)) == 2 };
};

template<bool b> struct incr_if_b { // default case: true
	template<class Itr>
	static Itr& exec(Itr& i) { return i; }
};

template<> struct incr_if_b<false> { // don't increase = false -> do incr.
	template<class Itr>
	static Itr& exec(Itr& i) { return ++i; }
};

// on match, some regexp parsers may stop at the last sign and thus must be
// increased (e.g. simple checks like isalnum()). others, however, may stop
// behind the last matched char (e.g. while loops). @a incr_if increments the
// pointer when necessary
template<class C> struct incr_if {
	template<class Itr>
	static Itr& exec(Itr& i) { return incr_if_b<detect_dont_incr<C>::value>::exec(i); }
};

// special case: redirect
template<class A, class C1>
struct incr_if<action<A, C1> > {
	template<class Itr>
	static Itr& exec(Itr& i) { return incr_if<typename action<A, C1>::child_t>::exec(i); }
};

template<class C1=Nothing, class C2=Nothing, class C3=Nothing, class C4=Nothing,
	class C5=Nothing, class C6=Nothing, class C7=Nothing, class C8=Nothing>
struct multiple_base
{
};

template<class C>
struct equal_base : multiple_base<C> {};

template<class C, class Cur> struct result_of
{
};

/*
template<class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8,
	template<class, class, class, class, class, class, class, class> class C,
	class Cur> struct result_of<C<C1, C2, C3, C4, C5, C6, C7, C8>, Cur>
{
	typedef C1::result_of<Cur>::type type;
};
*/




template<class C1, class C2, class C3=Nothing, class C4=Nothing,
	class C5=Nothing, class C6=Nothing, class C7=Nothing,
	class C8=Nothing>
struct regex : public multiple_base<C1, C2, C3, C4, C5, C6, C7, C8>, parser
{
	template<class Itr, class T, class T2>
	// C1<regex>::result: The result from a regex<C1> match
	constexpr static bool match_typed(Itr& i, T& result,
		T2 from_c1) // TODO: is copy by value good?
		//typename T::template result<C1>::type from_c1 = typename T::template result<C1>::type()) {
		{
		//typename T::template result<>
		return C1::match(i, from_c1)
			&& (
			result.append(from_c1),
			regex<C2, C3, C4, C5, C6, C7, C8,
			Nothing>::match(incr_if<C1>::exec(i), result)
			);
	}

public:
	typedef regex<C1, C2, C3, C4, C5, C6, C7, C8> real_t;

	template<class Itr, class T>
	// C1<regex>::result: The result from a regex<C1> match
	constexpr static bool match(Itr& i, T& result) {
		return match_typed(i, result, T::mk_result(C1()));
		//typename T::template result<>
		/*return C1::match(i, from_c1)
			&& (result.append(from_c1), true)
			&& regex<C2, C3, C4, C5, C6, C7, C8,
			Nothing>::match(incr_if<C1>::exec(i), result);*/
	}
	static const bool dont_incr = true;
};

template<>
struct regex<Nothing, Nothing, Nothing, Nothing, Nothing, Nothing,
	Nothing, Nothing> : public match_const<true> {};

template<char C> struct to_raw { typedef raw<C> type; };
template<> struct to_raw<0> { typedef Nothing type; };

template<char C1, char C2, char C3, char C4, char C5, char C6,
	char C7, char C8>
struct size_until_n
{
	static const std::size_t value = 1 + size_until_n<C2, C3, C4, C5, C6, C7, C8, 0>::value;
};

template<>
struct size_until_n <0,0,0,0,0,0,0,0>
{
	static const std::size_t value = 0;
};

template<char C1, char C2, char C3 = 0, char C4 = 0,
	char C5 = 0, char C6 = 0, char C7 = 0, char C8 = 0>
class str : public parser
{
	static const std::size_t sz = size_until_n<C1, C2, C3, C4, C5, C6, C7, C8>::value;
	static const char match_str[8];
public:
	typedef str<C1, C2, C3, C4, C5, C6, C7, C8> real_t;
	//typedef base real_t;
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& res) {
		return !strncmp(&*i, match_str, sz) // TODO: std::compare?
			&& (res = R(i, i + strlen(match_str)), i += strlen(match_str), true);
	}

	static const bool dont_incr = true;
};

template<char C1, char C2, char C3, char C4, char C5, char C6, char C7, char C8>
const char str<C1, C2, C3, C4, C5, C6, C7, C8>::match_str[8] = { C1, C2, C3, C4, C5, C6, C7, C8 };

template<class C1=Nothing, class C2=Nothing, class C3=Nothing, class C4=Nothing,
	class C5=Nothing , class C6=Nothing, class C7=Nothing,
	class C8=Nothing >
struct choices_base
{
};

template<class C1, class C2, class C3 = Nothing, class C4 = Nothing,
	class C5 = Nothing, class C6 = Nothing,
	class C7 = Nothing, class C8 = Nothing>
struct choices : public parser,
	public choices_base<C1, C2, C3, C4, C5, C6, C7, C8> 
{
	typedef choices<C1, C2, C3, C4, C5, C6, C7, C8> real_t;


	template<class Itr, class R, class R2>
	constexpr static bool match(Itr& i, R& res, R2 from_c1, Itr tmp = Itr()) {
		return ((tmp=i, C1::match(i, res) && (res.append(from_c1), incr_if<C1>::exec(i), true)))
			|| (i = tmp, choices<C2, C3, C4, C5, C6, C7, C8,
				Nothing>::match(i, res));
	}
public:
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& result) {
		return match(i, result, R::mk_result(C1()));
	}

	static const bool dont_incr = true;
};

template<>
struct choices<Nothing, Nothing, Nothing, Nothing, Nothing, Nothing,
	Nothing, Nothing> : public match_const<false> {};

template<char C1, char C2, char C3=0, char C4=0, char C5=0, char C6=0,
	char C7=0, char C8=0>
struct one_of : public parser, public choices_base<raw<C1>, raw<C2>, raw<C3>, raw<C4>,
	raw<C5>, raw<C6>, raw<C7>, raw<C8> > 
{
	typedef one_of<C1, C2, C3, C4, C5, C6, C7, C8> real_t;
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& res) {
		return raw<C1>::match(i, res) || one_of<C2, C3, C4, C5, C6, C7, C8,
			0>::match(i, res);
	}

	static const bool dont_incr = true;
};

template<>
struct one_of<0,0,0,0,0,0,0,0> : public match_const<false> {};

inline constexpr bool c_isdigit(const char c) {
	return c >= '0' && c <= '9';
}

inline constexpr bool c_isxdigit(const char c) {
	return c_isdigit(c)
		|| (c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f');
}

struct r_isxdigit : public basic<r_isxdigit> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) { return c_isxdigit(*i); }
};

struct r_isdigit : public basic<r_isdigit> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) { return c_isdigit(*i); }
};

struct r_isintsgn : public basic<r_isintsgn> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) {
		return *i == 'u' || *i == 'U' || *i == 'l' || *i == 'L'; // TODO: 4 indirections?
	}
};

template<char C>
struct r_isnot : public basic< r_isnot<C> > {
	template<class Itr>
	constexpr static bool ok(const Itr& i) {
		return *i != C;
	}
};

struct r_iscletter : public basic<r_iscletter>
{
	template<class Itr>
	constexpr static bool ok(const Itr& i) {
		return *i == '_' || (bool)isalnum(*i);
	}
};

struct r_iscletter1 : public basic<r_iscletter1>
{
	template<class Itr>
	constexpr static bool ok(const Itr& i) {
		return *i == '_' || (bool)isalpha(*i);
	}
};

template<class C>
class kleene : public equal_base<C>, public parser
{
	template<class Itr, class R, class T2>
	constexpr static bool match_typed(Itr& i, R& result, T2 from_c1)
	{
		return C::match(i, result) &&
			(result.append(from_c1),
			match(incr_if<C>::exec(i), result)), true; // TODO: true: a bit inefficient?
	}
public:
	typedef kleene<C> real_t;
	
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& result) {
#ifndef USE_CPP11
		std::cerr << "at: " << (*i) << std::endl;
#endif
		return match_typed(i, result, R::mk_result(C()));
	}
	static const bool dont_incr = true;
};

template<class C>
class kleene_p : public equal_base<C>, public parser
{
	template<class Itr, class R, class R2>
	constexpr static bool match(Itr& i, R& result, R2 from_c1) {
		return C::match(i, result) &&
			(result.append(from_c1),
			kleene<C>::match(incr_if<C>::exec(i), result));
	}
public:
	typedef kleene_p<C> real_t;
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& result) {
		return match(i, result, R::mk_result(C()));
	}
	static const bool dont_incr = true;
};

template<class C>
class maybe : public equal_base<C>, public parser
{
	template<class Itr, class R, class R2>
	constexpr static bool match(Itr& i, R& result, R2 from_c1) {
		return C::match(i, result) &&
			(result.append(from_c1),
			incr_if<C>::exec(i), true), true; // TODO: use incr_if here, too?
	}
public:
	typedef maybe<C> real_t;
	template<class Itr, class R>
	constexpr static bool match(Itr& i, R& result) {
		return match(i, result, R::mk_result(C()));
	}
	static const bool dont_incr = true;
};

class debugger {
public:
	template<class T, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
	static void collect(const T&, const multiple_base<C1, C2, C3, C4, C5, C6, C7, C8>* ) {
		std::cerr << "collecting kleene: " << "?" << std::endl;
	}
	template<class T, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
	static void collect(const T&, const choices_base<C1, C2, C3, C4, C5, C6, C7, C8>* ) {
		std::cerr << "collecting kleene: " << "?" << std::endl;
	}

public:
	typedef bool result;
};

#endif // TOKEN_H


