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

#ifndef C_GRAMMAR_H
#define C_GRAMMAR_H

#include "token.h"

// TODO: forward header?
class expression;
class argument_expression_list;
class assignment_expression;
class unary_operator;
class cast_expression;
class assignment_operator;
class declaration_specifiers;
class init_declarator_list;
class storage_class_specifier;
class type_specifier;
class type_qualifier;
class init_declarator;
class declarator;
class initializer;
class struct_or_union_specifier;
class enum_specifier;
class struct_or_union;
class struct_declaration_list;
class struct_declaration;
class specifier_qualifier_list;
class struct_declarator_list;
class struct_declarator;
class constant_expression;
class enumerator_list;
class enumerator;
class pointer;
class direct_declarator;
class parameter_type_list;
class identifier_list;
class type_qualifier_list;
class parameter_list;
class parameter_declaration;
class abstract_declarator;
class direct_abstract_declarator;
class initializer_list;
class labeled_statement;
class compound_statement;
class expression_statement;
class selection_statement;
class iteration_statement;
class jump_statement;
class statement_list;
class declaration_list;
class external_declaration;
class function_declaration;
class function_definition;

class primary_expression : public
	choices <
		one_of<
			identifier,
			constant,
			string_literal
		>,
		regex<raw<lpar>, expression, raw<rpar> >
	> {};

class postfix_expression : public
	choices <
		primary_expression,
		regex<postfix_expression, raw<lbrack>, expression, raw<rbrack> >,
		regex<postfix_expression, str<lpar, rpar> >,
		regex<postfix_expression, raw<lpar>, argument_expression_list, raw<rpar> >,
		regex<postfix_expression, str<point, identifier> >,
		regex<postfix_expression, str<ptr_op, identifier> >,
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
		regex<raw<k_sizeof>, unary_expression>,
		str<k_sizeof, lpar, type_name, rpar>
	> {};

class unary_operator : public
	one_of <
		and_sgn,
		star,
		plus_sgn,
		minus_sgn,
		tilde,
		excl_mark
	> {};

class cast_expression : public
	choices <
		unary_expression,
		regex<str<lpar, type_name, rpar>, cast_expression>
	> {};

class multiplicative_expression : public
	choices <
		cast_expression,
		regex<multiplicative_expression, raw<star>, cast_expression>,
		regex<multiplicative_expression, raw<slash>, cast_expression>,
		regex<multiplicative_expression, raw<percent>, cast_expression>
	> {};

class additive_expression : public
	choices <
		multiplicative_expression,
		regex<additive_expression, raw<plus_sgn>, multiplicative_expression>,
		regex<additive_expression, raw<minus_sgn>, multiplicative_expression>
	> {};

class shift_expression : public
	choices <
		additive_expression,
		regex<shift_expression, raw<left_op>, additive_expression>,
		regex<shift_expression, raw<right_op>, additive_expression>
	> {};

class relational_expression : public
	choices <
		shift_expression,
		regex<relational_expression, raw<langle>, shift_expression>,
		regex<relational_expression, raw<rangle>, shift_expression>,
		regex<relational_expression, raw<le_op>, shift_expression>,
		regex<relational_expression, raw<ge_op>, shift_expression>
	> {};

class equality_expression : public
	choices <
		relational_expression,
		regex<equality_expression, raw<eq_op>, relational_expression>,
		regex<equality_expression, raw<ne_op>, relational_expression>
	> {};

class and_expression : public
	choices <
		equality_expression,
		regex<and_expression, raw<and_sgn>, equality_expression>
	> {};

class exclusive_or_expression : public
	choices <
		and_expression,
		regex<exclusive_or_expression, raw<circ>, and_expression>
	> {};

class inclusive_or_expression : public
	choices <
		exclusive_or_expression,
		regex<inclusive_or_expression, raw<vbar>, exclusive_or_expression>
	> {};

class logical_and_expression : public
	choices <
		inclusive_or_expression,
		regex<logical_and_expression, raw<and_op>, inclusive_or_expression>
	> {};

class logical_or_expression : public
	choices <
		logical_and_expression,
		regex<logical_or_expression, raw<or_op>, logical_and_expression>
	> {};

class conditional_expression : public
	choices <
		logical_or_expression,
		regex<logical_or_expression, raw<qu_mark>, expression, raw<colon>, conditional_expression>
	> {};

class assignment_expression : public
	choices <
		conditional_expression,
		regex<unary_expression, assignment_operator, assignment_expression>
	> {};


class assignment_operator_1 : public
	one_of <
		eq_sgn,
		mul_assign,
		div_assign,
		mod_assign,
		add_assign,
		sub_assign
	> {};

class assignment_operator_2 : public
	one_of <
		left_assign,
		right_assign,
		and_assign,
		xor_assign,
		or_assign
	> {};


class assignment_operator : public
	choices <
		assignment_operator_1,
		assignment_operator_2
	> {};

class expression : public
	choices <
		assignment_expression,
		regex<expression, raw<comma>, assignment_expression>
	> {};

class rawconstant_expression : public
	conditional_expression {};

class declaration : public
	choices <
		regex<declaration_specifiers, raw<semicolon> >,
		regex<declaration_specifiers, init_declarator_list, raw<semicolon> >
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
		regex<declarator, raw<eq_sgn>, initializer>
	> {};

class storage_class_specifier : public
	one_of <
		k_typedef,
		k_extern,
		k_static,
		k_auto,
		k_register
	> {};

class type_specifier_1 : public
	one_of <
		k_void,
		k_char,
		k_short,
		k_int,
		k_long,
		k_float,
		k_double
	> {};

class type_specifier_2 : public
	choices <
		raw<k_signed>,
		raw<k_unsigned>,
		struct_or_union_specifier,
		enum_specifier,
		raw<type_name>
	> {};

class type_specifier : public
	choices <
		type_specifier_1,
		type_specifier_2
	> {};

class struct_or_union_specifier : public
	choices <
		regex<struct_or_union, str<identifier, lbrace>, struct_declaration_list, raw<rbrace> >,
		regex<struct_or_union, raw<lbrace>, struct_declaration_list, raw<rbrace> >,
		regex<struct_or_union, raw<identifier> >
	> {};

class struct_or_union : public
	one_of <
		k_struct,
		k_union
	> {};

class struct_declaration_list : public
	choices <
		struct_declaration,
		regex<struct_declaration_list, struct_declaration>
	> {};

class struct_declaration : public
	regex<specifier_qualifier_list, struct_declarator_list, raw<semicolon> > {};

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
		regex<raw<colon>, constant_expression>,
		regex<declarator, raw<colon>, constant_expression>
	> {};

class enum_specifier : public
	choices <
		regex<str<k_enum, lbrace>, enumerator_list, raw<rbrace> >,
		regex<str<k_enum, identifier, lbrace>, enumerator_list, raw<rbrace> >,
		str<k_enum, identifier>
	> {};

class enumerator_list : public
	choices <
		enumerator,
		regex<enumerator_list, raw<comma>, enumerator>
	> {};

class enumerator : public
	choices <
		raw<identifier>,
		regex<str<identifier, eq_sgn>, constant_expression>
	> {};

class type_qualifier : public
	one_of <
		k_const,
		k_volatile
	> {};

class declarator : public
	choices <
		regex<pointer, direct_declarator>,
		direct_declarator
	> {};

class direct_declarator : public
	choices <
		raw<identifier>,
		regex<raw<lpar>, declarator, raw<rpar> >,
		regex<direct_declarator, raw<lbrack>, constant_expression, raw<rbrack> >,
		regex<direct_declarator, str<lbrack, rbrack> >,
		regex<direct_declarator, raw<lpar>, parameter_type_list, raw<rpar> >,
		regex<direct_declarator, raw<lpar>, identifier_list, raw<rpar> >,
		regex<direct_declarator, str<lpar, rpar> >
	> {};

class pointer : public
	choices <
		raw<star>,
		regex<raw<star>, type_qualifier_list>,
		regex<raw<star>, pointer>,
		regex<raw<star>, type_qualifier_list, pointer>
	> {};

class type_qualifier_list : public
	choices <
		type_qualifier,
		regex<type_qualifier_list, type_qualifier>
	> {};


class parameter_type_list : public
	choices <
		parameter_list,
		regex<parameter_list, raw<comma>, raw<ellipsis> >
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

class identifier_list : public
	choices <
		raw<identifier>,
		regex<identifier_list, str<comma, identifier> >
	> {};

class type_name : public
	choices <
		specifier_qualifier_list,
		regex<specifier_qualifier_list, abstract_declarator>
	> {};

class abstract_declarator : public
	choices <
		pointer,
		direct_abstract_declarator,
		regex<pointer, direct_abstract_declarator>
	> {};

class direct_abstract_declarator_1 : public
	choices <
		regex<raw<lpar>, abstract_declarator, raw<rpar> >,
		str<lbrack, rbrack>,
		regex<raw<lbrack>, constant_expression, raw<rbrack> >,
		regex<direct_abstract_declarator, str<lbrack, rbrack> >,
		regex<direct_abstract_declarator, raw<lbrack>, constant_expression, raw<rbrack> >
	> {};

class direct_abstract_declarator_2 : public
	choices <
		str<lpar, rpar>,
		regex<raw<lpar>, parameter_type_list, raw<rpar> >,
		regex<direct_abstract_declarator, str<lpar, rpar> >,
		regex<direct_abstract_declarator, raw<lpar>, parameter_type_list, raw<rpar> >
	> {};

class direct_abstract_declarator : public
	choices <
		direct_abstract_declarator_1,
		direct_abstract_declarator_2
	> {};

class initializer : public
	choices <
		assignment_expression,
		regex<raw<lbrace>, initializer_list, raw<rbrace> >,
		regex<raw<lbrace>, initializer_list, str<comma, rbrace> >
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
		regex<str<identifier, colon>, statement>,
		regex<raw<k_case>, constant_expression, raw<colon>, statement>,
		regex<str<k_default, colon>, statement>
	> {};

class compound_statement : public
	choices <
		str<lbrace, rbrace>,
		regex<raw<lbrace>, statement_list, raw<rbrace> >,
		regex<raw<lbrace>, declaration_list, raw<rbrace> >,
		regex<raw<lbrace>, declaration_list, statement_list, raw<rbrace> >
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
		raw<semicolon>,
		regex<expression, raw<semicolon> >
	> {};

class selection_statement : public
	choices <
		regex<str<k_if, lpar>, expression, raw<rpar>, statement>,
		regex<str<k_if, lpar>, expression, raw<rpar>, statement, raw<k_else>, statement>,
		regex<str<k_switch, lpar>, expression, raw<rpar>, statement>
	> {};

class iteration_statement : public
	choices <
		regex<str<k_while, lpar>, expression, raw<rpar>, statement>,
		regex<raw<k_do>, statement, str<k_while, lpar>, expression, str<rpar, semicolon> >,
		regex<str<k_for, lpar>, expression_statement, expression_statement, raw<rpar>, statement>,
		regex<str<k_for, lpar>, expression_statement, expression_statement, expression, raw<rpar>, statement>
	> {};

class jump_statement : public
	choices <
		str<k_goto, identifier, semicolon>,
		str<k_continue, semicolon>,
		str<k_break, semicolon>,
		str<k_return, semicolon>,
		regex<raw<k_return>, expression, raw<semicolon> >
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


