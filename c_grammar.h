#ifndef C_GRAMMAR_H
#define C_GRAMMAR_H

#include "token.h"

class expression;
class argument_expression_list;
class assignment_expression;
class unary_operator;

class primary_expression : public
	choices <
		raw<identifier>,
		raw<constant>,
		raw<string_literal>,
		regex<raw<lpar>, expression, raw<rpar> >
	> {};

class postfix_expression : public
	choices <
		primary_expression,
		regex<postfix_expression, raw<lbrack>, expression, raw<rbrack> >,
		regex<postfix_expression, raw<lpar>, raw<rpar> >,
		regex<postfix_expression, raw<lpar>, argument_expression_list, raw<rpar> >,
		regex<postfix_expression, raw<point>, raw<identifier> >,
		regex<postfix_expression, raw<ptr_op>, raw<identifier> >,
		regex<postfix_expression, raw<inc_op> >,
		regex<postfix_expression, raw<dec_op> >
	> {};

class argument_expression_list : public
	choices <
		assignment_expression,
		regex<argument_expression_list, raw<comma>, assignment_expression>
	> {};

class unary_expression : public
	choices <
		postfix_expression,
		regex<raw<inc_op>, unary_expression>,
		regex<raw<dec_op>, unary_expression>,
		regex<unary_operator, cast_expression>,
		regex<k_sizeof, unary_expression>,
		regex<k_sizeof, raw<lpar>, type_name, raw<rpar> >
	> {};

class unary_operator : public
	choices <
		and_sgn,
		star,
		plus,
		minus,
		tilde,
		excl_mark
	> {};

class cast_expression : public
	choices <
		unary_expression,
		regex<raw<lpar>, type_name, raw<rpar>, cast_expression>
	> {};

class multiplicative_expression : public
	choices <
		cast_expression,
		regex<multiplicative_expression, star, cast_expression>,
		regex<multiplicative_expression, slash, cast_expression>,
		regex<multiplicative_expression, percent, cast_expression>
	> {};

class additive_expression : public
	choices <
		multiplicative_expression,
		regex<additive_expression, plus, multiplicative_expression>,
		regex<additive_expression, minus, multiplicative_expression>
	> {};

class shift_expression : public
	choices <
		additive_expression,
		regex<shift_expression, left_op, additive_expression>,
		regex<shift_expression, right_op, additive_expression>
	> {};

class relational_expression : public
	choices <
		shift_expression,
		regex<relational_expression, langle, shift_expression>,
		regex<relational_expression, rangle, shift_expression>,
		regex<relational_expression, le_op, shift_expression>,
		regex<relational_expression, ge_op, shift_expression>
	> {};

class equality_expression : public
	choices <
		relational_expression,
		regex<equality_expression, eq_op, relational_expression>,
		regex<equality_expression, ne_op, relational_expression>
	> {};

class and_expression : public
	choices <
		equality_expression,
		regex<and_expression, and_sgn, equality_expression>
	> {};

class exclusive_or_expression : public
	choices <
		and_expression,
		regex<exclusive_or_expression, circ, and_expression>
	> {};

class inclusive_or_expression : public
	choices <
		exclusive_or_expression,
		regex<inclusive_or_expression, pipe, exclusive_or_expression>
	> {};

class logical_and_expression : public
	choices <
		inclusive_or_expression,
		regex<logical_and_expression, and_op, inclusive_or_expression>
	> {};

class logical_or_expression : public
	choices <
		logical_and_expression,
		regex<logical_or_expression, or_op, logical_and_expression>
	> {};

class conditional_expression : public
	choices <
		logical_or_expression,
		regex<logical_or_expression, qu_mark, expression, colon, conditional_expression>
	> {};

class assignment_expression : public
	choices <
		conditional_expression,
		regex<unary_expression, assignment_operator, assignment_expression>
	> {};

class assignment_operator : public
	choices <
		eq_sgn,
		mul_assign,
		div_assign,
		mod_assign,
		add_assign,
		sub_assign,
		left_assign,
		right_assign,
		and_assign,
		xor_assign,
		or_assign
	> {};

class expression : public
	choices <
		assignment_expression,
		regex<expression, raw<comma>, assignment_expression>
	> {};

class raw<constant>_expression : public
	conditional_expression {};

class declaration : public
	choices <
		regex<declaration_specifiers, semicolon>
		regex<declaration_specifiers, init_declarator_list, semicolon>
	> {};

class declaration_specifiers : public
	choices <
		storage_class_specifier,
		regex<storage_class_specifier, declaration_specifiers>,
		type_specifier,
		regex<type_specifier, declaration_specifiers>,
		type_qualifier,
		regex<type_qualifier, declaration_specifiers>
	> {};

class init_declarator_list : public
	choices <
		init_declarator,
		regex<init_declarator_list, raw<comma>, init_declarator>
	> {};

class init_declarator : public
	choices <
		declarator,
		regex<declarator, eq_sgn, initializer>
	> {};

class storage_class_specifier : public
	choices <
		k_typedef,
		k_extern,
		k_static,
		k_auto,
		k_register
	> {};

class type_specifier : public
	choices <
		k_void,
		k_char,
		k_short,
		k_int,
		k_long,
		k_float,
		k_double,
		k_signed,
		unk_signed,
		struct_or_union_specifier,
		enum_specifier,
		type_name
	> {};

class struct_or_union_specifier : public
	choices <
		regex<struct_or_union, raw<identifier>, lbrace, struct_declaration_list, rbrace>,
		regex<struct_or_union, lbrace, struct_declaration_list, rbrace>,
		regex<struct_or_union, raw<identifier> >
	> {};

class struct_or_union : public
	choices <
		k_struct,
		k_union
	> {};

class struct_declaration_list : public
	choices <
		struct_declaration,
		regex<struct_declaration_list, struct_declaration>
	> {};

class struct_declaration : public
	regex<specifier_qualifier_list, struct_declarator_list, semicolon>

class specifier_qualifier_list : public
	choices <
		regex<type_specifier, specifier_qualifier_list>,
		type_specifier,
		regex<type_qualifier, specifier_qualifier_list>,
		type_qualifier
	> {};

class struct_declarator_list : public
	choices <
		struct_declarator,
		regex<struct_declarator_list, raw<comma>, struct_declarator>
	> {};

class struct_declarator : public
	choices <
		declarator,
		regex<colon, raw<constant>_expression>,
		regex<declarator, colon, raw<constant>_expression>
	> {};

class enum_specifier : public
	choices <
		regex<k_enum, lbrace, enumerator_list, rbrace>,
		regex<k_enum, raw<identifier>, lbrace, enumerator_list, rbrace>,
		regex<k_enum, raw<identifier> >
	> {};

class enumerator_list : public
	choices <
		enumerator,
		regex<enumerator_list, raw<comma>, enumerator>
	> {};

class enumerator : public
	choices <
		raw<identifier>,
		regex<raw<identifier>, eq_sgn, raw<constant>_expression>
	> {};

class type_qualifier : public
	choices <
		k_const,
		k_volatile
	> {};

class declarator : public
	choices <
		regex<raw<point>er, direct_declarator>,
		direct_declarator
	> {};

class direct_declarator : public
	choices <
		raw<identifier>,
		regex<raw<lpar>, declarator, raw<rpar> >,
		regex<direct_declarator, raw<lbrack>, raw<constant>_expression, raw<rbrack> >,
		regex<direct_declarator, raw<lbrack>, raw<rbrack> >,
		regex<direct_declarator, raw<lpar>, parameter_type_list, raw<rpar> >,
		regex<direct_declarator, raw<lpar>, raw<identifier>_list, raw<rpar> >,
		regex<direct_declarator, raw<lpar>, raw<rpar> >
	> {};

class raw<point>er : public
	choices <
		star,
		regex<star, type_qualifier_list>,
		regex<star, raw<point>er>,
		regex<star, type_qualifier_list, raw<point>er>
	> {};

class type_qualifier_list : public
	choices <
		type_qualifier,
		regex<type_qualifier_list, type_qualifier>
	> {};


class parameter_type_list : public
	choices <
		parameter_list,
		regex<parameter_list, raw<comma>, ellipsis>
	> {};

class parameter_list : public
	choices <
		parameter_declaration,
		regex<parameter_list, raw<comma>, parameter_declaration>
	> {};

class parameter_declaration : public
	choices <
		regex<declaration_specifiers, declarator>,
		regex<declaration_specifiers, abstract_declarator>,
		declaration_specifiers
	> {};

class raw<identifier>_list : public
	choices <
		raw<identifier>,
		regex<raw<identifier>_list, raw<comma>, raw<identifier> >
	> {};

class type_name : public
	choices <
		specifier_qualifier_list,
		regex<specifier_qualifier_list, abstract_declarator>
	> {};

class abstract_declarator : public
	choices <
		raw<point>er,
		direct_abstract_declarator,
		regex<raw<point>er, direct_abstract_declarator>
	> {};

class direct_abstract_declarator : public
	choices <
		regex<raw<lpar>, abstract_declarator, raw<rpar> >,
		regex<raw<lbrack>, raw<rbrack> >,
		regex<raw<lbrack>, raw<constant>_expression, raw<rbrack> >,
		regex<direct_abstract_declarator, raw<lbrack>, raw<rbrack> >,
		regex<direct_abstract_declarator, raw<lbrack>, raw<constant>_expression, raw<rbrack> >,
		regex<raw<lpar>, raw<rpar> >,
		regex<raw<lpar>, parameter_type_list, raw<rpar> >,
		regex<direct_abstract_declarator, raw<lpar>, raw<rpar> >,
		regex<direct_abstract_declarator, raw<lpar>, parameter_type_list, raw<rpar> >
	> {};

class initializer : public
	choices <
		assignment_expression,
		regex<lbrace, initializer_list, rbrace>,
		regex<lbrace, initializer_list, raw<comma>, rbrace>
	> {};

class initializer_list : public
	choices <
		initializer,
		regex<initializer_list, raw<comma>, initializer>
	> {};

class statement : public
	choices <
		labeled_statement,
		compound_statement,
		expression_statement,
		selection_statement,
		iteration_statement,
		jump_statement
	> {};

class labeled_statement : public
	choices <
		regex<raw<identifier>, colon, statement>,
		regex<k_case, raw<constant>_expression, colon, statement>,
		regex<k_default, colon, statement>
	> {};

class compound_statement : public
	choices <
		regex<lbrace, rbrace>,
		regex<lbrace, statement_list, rbrace>,
		regex<lbrace, declaration_list, rbrace>,
		regex<lbrace, declaration_list, statement_list, rbrace>
	> {};

class declaration_list : public
	choices <
		declaration,
		regex<declaration_list, declaration>
	> {};

class statement_list : public
	choices <
		statement,
		regex<statement_list, statement>
	> {};

class expression_statement : public
	choices <
		semicolon,
		regex<expression, semicolon>
	> {};

class selection_statement : public
	choices <
		regex<k_if, raw<lpar>, expression, raw<rpar>, statement>,
		regex<k_if, raw<lpar>, expression, raw<rpar>, statement, k_else, statement>,
		regex<k_switch, raw<lpar>, expression, raw<rpar>, statement>
	> {};

class iteration_statement : public
	choices <
		regex<k_while, raw<lpar>, expression, raw<rpar>, statement>,
		regex<k_do, statement, k_while, raw<lpar>, expression, raw<rpar>, semicolon>,
		regex<k_for, raw<lpar>, expression_statement, expression_statement, raw<rpar>, statement>,
		regex<k_for, raw<lpar>, expression_statement, expression_statement, expression, raw<rpar>, statement>
	> {};

class jump_statement : public
	choices <
		regex<k_goto, raw<identifier>, semicolon>,
		regex<k_continue, semicolon>,
		regex<k_break, semicolon>,
		regex<k_return, semicolon>,
		regex<k_return, expression, semicolon>
	> {};

class translation_unit : public
	choices <
		external_declaration,
		regex<translation_unit, external_declaration>
	> {};

class external_declaration : public
	choices <
		function_definition,
		declaration
	> {};

class function_definition : public
	choices <
		regex<declaration_specifiers, declarator, declaration_list, compound_statement>,
		regex<declaration_specifiers, declarator, compound_statement>,
		regex<declarator, declaration_list, compound_statement>,
		regex<declarator, compound_statement>
	> {};

#endif // C_GRAMMAR_H


