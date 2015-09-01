#ifndef TOKEN_H
#define TOKEN_H

#include <map>

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

	k_case, k_default, k_if, k_else, k_switch, k_while, k_do, k_for, k_goto, k_continue, k_break, k_return
};


class token_map
{
	void init_tokens();
	std::map<const char*, token_type> tokens;
public:
	token_map();

};

class token_t
{
public:
	token_t();
};

#endif // TOKEN_H
