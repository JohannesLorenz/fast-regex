#include <iostream>
#include "token.h"

typedef std::vector<token_type>::const_iterator token_itr;

template<token_type >
class T {};

template<class T, class N>
struct tand
{
	typedef T cur;
	typedef N next;
};

template<class T, class N>
struct tor
{
	typedef T cur;
	typedef N next;
};

template<class Tl> // Tl = and
struct match_rule;

template<class X, class Y>
struct match_rule<tor<X, Y> >;


// basic case: First is another rule
template<class First>
struct match_first {
	static bool match(token_itr& i, const token_itr& end) {
	//	return First::match(i);
		return match_rule<First>::match(i, end);
	}
};

template<token_type TokenId>
struct match_first<T<TokenId> > {
	static bool match(token_itr& i, const token_itr& end) {
		return (*i == TokenId) && (++i, true);
	}
};

template<class Next>
bool recur(token_itr& i, const token_itr& end);

template<>
bool recur<void>(token_itr& i, const token_itr& end);

template<class Tl> // Tl = and
struct match_rule {
	static bool match(token_itr& i, const token_itr& end) {

		std::cerr << "..." << std::endl;
		return match_first<typename Tl::cur>::match(i, end)
			&& recur<typename Tl::next>(i, end);
	}
};

template<class X, class Y>
struct match_rule<tor<X, Y> >
{
	typedef tor<X, Y> T;
	static bool match(token_itr& i, const token_itr& end) {
		return match_rule<typename T::cur>::match(i, end)
			|| recur<typename T::next>(i, end);
	}
};

template<class Next>
bool recur(token_itr& i, const token_itr& end) {
	return match_rule<Next>::match(i, end); }

template<>
bool recur<void>(token_itr& i, const token_itr& end) {
	// reached the end s of template list successfully
	// is this also the end of the token vector?
	return i == end; }

struct t2;

struct t1 : public tand<T<k_volatile>, tand<t2, void> > { tyipedef tand<T<k_volatile>, tand<t2, void> > base; };
struct t2 : public tand<t1, void> { typedef tand<t1, void> base; };

void test() {
/*	std::vector<token_type> tv;
	tv.push_back(k_const);
	tv.push_back(k_volatile);
	typedef tor< tand<T<k_const>, tand<T<k_volatile>, void> >,
			tand<T<k_volatile>, void>
		>
			tand1;
	token_itr tmp = tv.begin();
	std::cerr << "t1:" << match_rule<tand1>::match(tmp, tv.end());

	std::vector<token_type> tv2;
	tv2.push_back(k_const);
	tv2.push_back(k_const);
	tmp = tv2.begin();
	std::cerr << "t2:" << match_rule<tand1>::match(tmp, tv2.end());

	std::vector<token_type> tv3;
	tv3.push_back(k_volatile);
	tmp = tv3.begin();
	std::cerr << "t3:" << match_rule<tand1>::match(tmp, tv3.end()) << std::endl;
*/
	std::vector<token_type> tv4;
	token_itr tmp = tv4.begin();
	tv4.push_back(k_volatile);
	tv4.push_back(k_volatile);
	tv4.push_back(k_volatile);
	tv4.push_back(k_volatile);
	tv4.push_back(k_volatile);
	tmp = tv4.begin();
	std::cerr << "t4:" << match_rule<t1>::match(tmp, tv4.end()) << std::endl;


}

int main(void) {
	test(); return 0;
}



/*
template<class C1>
class rule1 {
	ret
};

template<class C1, class C2>
class rule2 : rule1<C1> {
	bool match(tok_itr& i) {
		rule1<>
	}
};
*/
