#ifndef TOKEN_H
#define TOKEN_H

#include <typeinfo>
#include <cstring>
#include <ostream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
enum token_type
{
	identifier, constant, string_literal, k_sizeof,
	ptr_op, inc_op, dec_op, left_op, right_op, le_op, ge_op, eq_op, ne_op,
	and_op, or_op, mul_assign, div_assign, mod_assign, add_assign,
	sub_assign, left_assign, right_assign, and_assign,
	xor_assign, or_assign, type_name,

	k_typedef, k_extern, k_static, k_auto, k_register,
	k_char, k_short, k_int, k_long, k_signed, k_unsigned, k_float, k_double, k_const, k_volatile, k_void,
	k_struct, k_union, k_enum, ellipsis,

	k_case, k_default, k_if, k_else, k_switch, k_while, k_do, k_for, k_goto, k_continue, k_break, k_return,
	none
};


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

	operator bool() const { return type != none; }

	friend std::ostream& operator<<(std::ostream& o, token_t& t);
	std::ostream& dump(std::ostream& o, const std::string& file);
};

inline std::ostream& operator<<(std::ostream& o, token_t& t) {
	return o << "(token_id:) " << (int)t.type;
}

class token_reader_base
{
	token_type type;
protected:
	typedef std::string::const_iterator iterator;
public:
	static token_t invalid(const std::string::const_iterator& i) {
		return token_t(none, i, i);
	}
	token_t matches(const std::string& s, iterator i) const {
		iterator start = i;
		bool ok = match(i);
		if(!ok)
		 return invalid(i);
		else
		 return token_t(type, start, i);
	}
	virtual bool match(iterator& i) const = 0;

	token_reader_base(token_type type) :
		type(type)
	{
	}

};

class token_reader : public token_reader_base
{
	const char* match_str;
private:
	virtual bool match(iterator& i) const {
		std::size_t len = strlen(match_str);
		if(!strncmp(&*i, match_str, len)) // TODO: virtual...
			return i+=len, true;
		else
			return false; // TODO: if-else not needed
	
	}
public:
	token_reader(const char* match_str, token_type type) :
		token_reader_base(type),
		match_str(match_str)
	{
	}
/*	bool eq(const char* s) { bool match = !strncmp(*recent_start, s, strlen(s));
		if(match) recent_end = recent_start + strlen(s);
	}*/
};

template<class R>
class token_reader_r : public token_reader_base
{
private:
	virtual bool match(iterator& i) const {
		return R::match(i);
	}
public:
	token_reader_r(token_type type) :
		token_reader_base(type) {}
};

typedef bool (*char_fptr)(char);

#if 0
// NOT TESTED YET!
//! @param c must not be 0
//! @param choices must be 0 terminated
inline bool one_of(char c, const char* choices)
{
	for(; *choices && (*choices != c); ++choices) ;
	return (bool)*choices;
}
#endif

template<char C>
inline bool is_not(char c) { return c != C; }

inline bool cletter1(char c) {
	return c == '_' || (bool)isalpha(c);
}

inline bool cletter(char c) {
	return c == '_' || (bool)isalnum(c);
}

#if 0
// NOT TESTED YET!
inline bool once_or_not(std::string::const_iterator& i, char_fptr fptr) {
	return fptr(*i) && ++i, true;
}
#endif

class regex_base // TODO: parse_base?
{
protected:
	typedef std::string::const_iterator iterator;
};

template<char C>
class raw : regex_base {
public:
	static bool match(iterator& i) { return *i == C; }
};

//! represents that nothing more is to be parsed.
class Nothing : regex_base {
public:
	static bool match(iterator& i) { throw "can not match class \"Nothing\""; }
};

template<class C> struct kleenee;
template<class C> struct kleenee_p;
template<class C> struct maybe;
template<class C1, class C2=Nothing, class C3=Nothing, class C4=Nothing,
	class C5 = Nothing, class C6=Nothing, class C7=Nothing,
	class C8 = Nothing>
class regex;

template<class T> struct mk_debug;
template<> struct mk_debug<Nothing>;

template<class Terminal> class debug : regex_base {
public:
	typedef Terminal child_t;
	static bool match(iterator& i) {
		std::cerr << "trying to match terminal:" << std::endl << typeid(Terminal).name() << std::endl;
		bool res = Terminal::match(i);
		if(res)
			std::cerr << " -> success (terminal)!" << std::endl;
		return res;

	}
};
template<template<class > class T, class C1>
class debug<T<C1> > : regex_base {
public:
	typedef T< typename  mk_debug<C1>::type > child_t;
	static bool match(iterator& i) {
		std::cerr << "trying to match:" << std::endl << typeid(T<C1>).name() << std::endl;
		bool res = child_t::match(i);
		if(res)
			std::cerr << " -> success!" << std::endl;
		return res;

	}
};

template<template<class, class, class, class, class, class, class, class > class T,
	class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
class debug<T<C1, C2, C3, C4, C5, C6, C7, C8> > : regex_base {
public:
	typedef T< typename mk_debug<C1>::type, typename mk_debug<C2>::type,
		typename mk_debug<C3>::type, typename mk_debug<C4>::type,
		typename mk_debug<C5>::type, typename mk_debug<C6>::type,
		typename mk_debug<C7>::type, typename mk_debug<C8>::type
		>child_t;
	static bool match(iterator& i) {
		// TODO: do not print "Nothing"s?
		std::cerr << "trying to match:" << std::endl << typeid(T<C1, C2, C3, C4, C5, C6, C7, C8>).name() << std::endl;
		bool res = child_t::match(i);
		if(res)
			std::cerr << " -> success!" << std::endl;
		return res;
	}
};

// do not produce debug<Nothing> (this would let classes that assume "Nothing" fail
// also, debug output for Nothing should be avoided
template<class T> struct mk_debug {
	typedef debug<T> type;
};
template<> struct mk_debug<Nothing> {
	typedef Nothing type;
};

template<class C> struct incr_if {
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return ++i; }
};

template<class C> struct incr_if<kleenee<C> > {
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return i; }
};


template<class C> struct incr_if<kleenee_p<C> > { // TODO: use sfinae
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return i; }
};
template<class C> struct incr_if<maybe<C> > { // TODO: use sfinae
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return i; }
};
template<class C1, class C2, class C3, class C4, class C5, class C6,
	class C7, class C8>
	struct incr_if<regex<C1, C2, C3, C4, C5, C6, C7, C8> > { // TODO: use sfinae
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return i; }
};
template<class C1>
struct incr_if<debug<C1> > {
	typedef std::string::const_iterator iterator;
	static iterator& exec(iterator& i) { return incr_if<typename debug<C1>::child_t>::exec(i); }
};

/*template<class C1, class C2=Nothing, class C3=Nothing, class C4=Nothing,
	class C5 = Nothing, class C6=Nothing, class C7=Nothing,
	class C8 = Nothing>*/
template<class C1, class C2, class C3, class C4,
	class C5, class C6, class C7,
	class C8>
class regex : regex_base
{
public:
	static bool match(iterator& i) {
		return C1::match(i) && regex<C2, C3, C4, C5, C6, C7, C8,
			Nothing>::match(incr_if<C1>::exec(i));
	}
};

template<>
class regex<Nothing, Nothing, Nothing, Nothing, Nothing, Nothing,
	Nothing, Nothing> : regex_base
{
public:
	static bool match(const iterator& ) {
		return true;
	}
};

template<char C> struct to_raw { typedef raw<C> type; };
template<> struct to_raw<0> { typedef Nothing type; };

template<char C1, char C2, char C3=0, char C4=0, char C5=0, char C6=0,
	char C7=0, char C8=0>
class str : regex_base
{
	typedef regex<
		to_raw<C1>, to_raw<C2>,
		to_raw<C3>, to_raw<C4>,
		to_raw<C5>, to_raw<C6>,
		to_raw<C7>, to_raw<C8>
	>
	base;
public:
	static bool match(const iterator& i) {
		return base::match(i);
	}
};

template<class C1, class C2=Nothing, class C3=Nothing, class C4=Nothing,
	class C5 = Nothing, class C6=Nothing, class C7=Nothing,
	class C8 = Nothing>
class choices : regex_base
{
public: // TODO: copy itr
	static bool match(iterator& i) {
		return C1::match(i) || choices<C2, C3, C4, C5, C6, C7, C8,
			Nothing>::match(i);
	}
};

template<>
class choices<Nothing, Nothing, Nothing, Nothing, Nothing, Nothing,
	Nothing, Nothing> : regex_base
{
public:
	static bool match(const iterator& ) {
		return false;
	}
};

template<char C1, char C2=0, char C3=0, char C4=0, char C5=0, char C6=0,
	char C7=0, char C8=0>
class one_of : regex_base
{
public:
	static bool match(iterator& i) {
		return raw<C1>::match(i) || one_of<C2, C3, C4, C5, C6, C7, C8,
			0>::match(i);
	}
};

template<>
class one_of<0,0,0,0,0,0,0,0> : regex_base
{
public:
	static bool match(const iterator& ) {
		return false;
	}
};


struct r_isxdigit : regex_base
{
	static bool match(const iterator& i) {
		return isxdigit(*i);
	}
};

struct r_isdigit : regex_base
{
	static bool match(const iterator& i) {
		return isdigit(*i);
	}
};

struct r_isintsgn : regex_base
{
	static bool match(const iterator& c) {
		return *c == 'u' || *c == 'U' || *c == 'l' || *c == 'L'; // TODO: 4 indirections?
	}
};

template<char C>
struct r_isnot : regex_base
{
	static bool match(const iterator& i) {
		return *i != C;
	}
};

struct r_iscletter : regex_base
{
	static bool match(const iterator& i) {
		return *i == '_' || (bool)isalnum(*i);
	}
};

struct r_iscletter1 : regex_base
{
	static bool match(const iterator& i) {
		return *i == '_' || (bool)isalpha(*i);
	}
};

template<class C>
struct kleenee : regex_base
{
	static bool match(iterator& i) {
		return C::match(i) && match(++i), true; // TODO: true: a bit inefficient?
	}
};

template<class C>
struct kleenee_p : regex_base
{
	static bool match(iterator& i) {
		return C::match(i) && kleenee<C>::match(++i);
	}
};

template<class C>
struct maybe : regex_base
{
	static bool match(iterator& i) {
		return C::match(i) && (incr_if<C>::exec(i), true), true; // TODO: use incr_if here, too?
	}
};

template<class Regex>
void assert_match(const char* str, std::size_t digits) {
	std::string s = str;
	std::string::const_iterator itr0 = s.begin(), itr = s.begin();
	Regex::match(itr);
	std::cerr << std::distance(itr0, itr) << std::endl;
	if(std::distance(itr0, itr) != digits)
	{
		std::cerr << "at: " << ((&*itr)-2) << std::endl;
		 throw "regex test error";
	}
}

template<class Regex>
void assert_match(const char* str) {
	assert_match<Regex>(str, strlen(str));
}

typedef regex<	raw<'0'>,
		one_of<'x', 'X'>,
		kleenee_p<r_isxdigit>,
		kleenee<r_isintsgn>
	> hex_const;
	
typedef regex<	raw<'0'>,
		kleenee_p<r_isdigit>,
		kleenee<r_isintsgn>
	> r_oct_const;

typedef regex<	kleenee_p<r_isdigit>,
		kleenee<r_isintsgn>
	> r_dec_const;

typedef regex<	maybe< raw<'L'> >,
		raw<'\''>,
		kleenee_p< r_isnot<'\''> >,
		raw<'\''>
	> r_char_const;

typedef regex<	one_of<'e', 'E'>,
		maybe< one_of<'+', '-'> >
	> r_exp_part;

typedef regex<	r_iscletter1,
		kleenee<r_iscletter>
	> r_identifier;

typedef regex<	one_of<'f', 'F', 'l', 'L'> > r_floatsgn;

typedef regex<	kleenee_p<r_isdigit>,
		r_exp_part,
		maybe<r_floatsgn>
	> r_float_const;

typedef regex<	kleenee<r_isdigit>,
		raw<'.'>,
		kleenee_p<r_isdigit>,
		maybe<r_exp_part>,
		maybe<r_floatsgn>
	> r_float_const_2;

typedef regex<	kleenee_p<r_isdigit>,
		raw<'.'>,
		kleenee<r_isdigit>,
		maybe<r_exp_part>,
		maybe<r_floatsgn>
	> r_float_const_3;

typedef regex<	maybe< raw<'L'> >,
		raw<'"'>,
		kleenee_p< r_isnot<'"'> >,
		raw<'"'>
	> r_string_literal;

inline void test_regex()
{
	assert_match<hex_const>("0xA1");
	assert_match<hex_const>("0xA1ul");
	
	assert_match<r_string_literal>("L\"hello\""); // TODO: \" ?

	assert_match<r_float_const_2>(".52e+f");
	assert_match<r_float_const_2>(".52f");

	assert_match< debug<r_float_const_2> >(".52f");
};

class token_map
{
	void init_tokens();
	std::vector<token_reader*> tokens;
	// TODO: use maps
	void add(const char* match_str, token_type type) {
		tokens.push_back(new token_reader(match_str, type));
	}
public:
	token_map();
	token_t apply(const std::string& s, std::string::const_iterator& sitr)
	{
		token_t cur = token_reader::invalid(sitr);
		for(std::vector<token_reader*>::const_iterator itr = tokens.begin();
			itr != tokens.end() && !cur; ++itr)
		{
			cur = (*itr)->matches(s, sitr); // TODO: no braces?
		}
		sitr = cur.end();
		return cur;
	}

};

class cfile
{
	token_map map;
	std::vector<token_t> _tokens;
public:
	cfile(const std::string& content);
};

#endif // TOKEN_H

