#ifndef TOKEN_H
#define TOKEN_H

#include <typeinfo>
#include <cstring>
#include <ostream>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#ifdef USE_CPP11
#else
	#define constexpr
#endif


enum token_type
{
	no_token, // an invalid token, like \0 in strings

	identifier, constant, string_literal, k_sizeof,
	ptr_op, inc_op, dec_op, left_op, right_op, le_op, ge_op, eq_op, ne_op,
	and_op, or_op, mul_assign, div_assign, mod_assign, add_assign,
	sub_assign, left_assign, right_assign, and_assign,
	xor_assign, or_assign, type_name,

	k_typedef, k_extern, k_static, k_auto, k_register,
	k_char, k_short, k_int, k_long, k_signed, k_unsigned, k_float, k_double, k_const, k_volatile, k_void,
	k_struct, k_union, k_enum, ellipsis,

	k_case, k_default, k_if, k_else, k_switch, k_while, k_do, k_for, k_goto, k_continue, k_break, k_return,

	// "simple signs":
	// braces: {}, parantheses: (), brackets: [], angles: <>
	semicolon, lbrace,
	rbrace, comma, colon, eq_sgn, lpar, rpar, lbrack, rbrack,
	point, and_sgn, excl_mark, tilde, minus_sgn, plus_sgn, star, slash,
	percent, langle, rangle, circ, vbar, qu_mark, whitespace,

	none // TODO: deprecated?
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
	
	virtual ~token_reader_base() {} // TODO: = 0?
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

	virtual ~token_reader() {} // TODO: = 0?
};

template<class R>
class token_reader_r : public token_reader_base
{
	virtual bool match(iterator& i) const {
		return R::match(i);
	}
public:
	token_reader_r(token_type type) :
		token_reader_base(type) {}
};

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
			(res = T(start, i), true));
	}
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
		return !strncmp(&*i, match_str, sz); // TODO: std functions?
	}
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
		return (tmp=i, C1::match(i, res) && (res.append(from_c1), incr_if<C1>::exec(i), true))
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

/*struct r_isxdigit : public parser
{
	typedef r_isxdigit real_t;
	template<class Itr>
	constexpr static bool match(const Itr& i) {
		return c_isxdigit(*i);
	}
};*/

struct r_isxdigit : public basic<r_isxdigit> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) { return c_isxdigit(*i); }
};

/*struct r_isdigit : public parser
{
	typedef r_isdigit real_t;
	template<class Itr>
	constexpr static bool match(const Itr& i) {
		return c_isdigit(*i);
	}
};*/

struct r_isdigit : public basic<r_isdigit> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) { return c_isdigit(*i); }
};

/*struct r_isintsgn : public parser
{
	typedef r_isintsgn real_t;
	template<class Itr>
	constexpr static bool match(const Itr& c) {
		return *c == 'u' || *c == 'U' || *c == 'l' || *c == 'L'; // TODO: 4 indirections?
	}
};*/

struct r_isintsgn : public basic<r_isintsgn> {
	template<class Itr>
	constexpr static bool ok(const Itr& i) {
		return *i == 'u' || *i == 'U' || *i == 'l' || *i == 'L'; // TODO: 4 indirections?
	}
};

/*template<char C>
struct r_isnot : public parser
{
	typedef r_isnot<C> real_t;
	template<class Itr>
	constexpr static bool match(const Itr& i) {
		return *i != C;
	}
};

struct r_iscletter : public parser
{
	typedef r_iscletter real_t;
	template<class Itr>
	constexpr static bool match(const Itr& i) {
		return *i == '_' || (bool)isalnum(*i);
	}
};

struct r_iscletter1 : public parser
{
	typedef r_iscletter1 real_t;
	template<class Itr>
	constexpr static bool match(const Itr& i) {
		return *i == '_' || (bool)isalpha(*i);
	}
};*/

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

template<token_type Token, char C1, char C2 = 0, char C3 = 0, char C4 = 0,
	char C5 = 0, char C6 = 0, char C7 = 0, char C8 = 0>
class tok_str : public str<C1, C2, C3, C4, C5, C6, C7, C8> {}; // TODO: reuse tok_regex?

template<token_type Token, class C1, class C2 = Nothing,
	class C3 = Nothing, class C4 = Nothing,
	class C5 = Nothing, class C6 = Nothing,
	class C7 = Nothing, class C8 = Nothing>
class tok_regex : public regex<C1, C2, C3, C4, C5, C6, C7, C8> {};

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
		tok_regex<identifier, r_identifier>,
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

struct parse
{
	template<class T>
	static parse mk_result(const T& ) { return parse(); }

	template<class Itr1, class Itr2>
	parse(const Itr1&, const Itr2& ) {}
	parse() {}

	void append(const parse& ) const {}
};

template<class Regex, class Result>
void assert_match(const char* str, std::size_t digits) {
	std::string s = str;
	std::string::const_iterator itr0 = s.begin(), itr = s.begin();
	
	//typedef typename mk_action<debugger, Regex>::type ac_regex;
	typedef Regex ac_regex;
	Result res;
	if(! ac_regex::match(itr, res) )
	 std::cerr << "WARNING: did not match..." << std::endl;
	
	if((std::size_t)std::distance(itr0, itr) != digits)
	{
		std::cerr << std::distance(itr0, itr) << std::endl;
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

template<class Regex, class Result>
void assert_match(const char* str) {
	assert_match<Regex, Result>(str, strlen(str));
}

template<class Regex>
constexpr bool c_assert_match(const char* str) {
	return c_assert_match<Regex>(str, strlen(str));
}

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

	const char* str = "acb";
	string_res res;
	result_test::match(str, res);
	std::cerr << "my result: " << res.res << std::endl;
}

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


