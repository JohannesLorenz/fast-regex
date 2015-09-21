#ifndef C_GRAMMAR_H
#define C_GRAMMAR_H

#include "token.h"

class primary_expression : public
	choices <
		identifier,
		constant,
		string_literal,
		regex<'(', expression, ')'>,
	> {};

class postfix_expression : public
	choices <
		primary_expression,
		regex<postfix_expression, '[', expression, ']'>,
		regex<postfix_expression, '(', ')'>,
		regex<postfix_expression, '(', argument_expression_list, ')'>,
		regex<postfix_expression, '.', identifier>,
		regex<postfix_expression, ptr_op, identifier>,
		regex<postfix_expression, inc_op>,
		regex<postfix_expression, dec_op>,
	> {};

class argument_expression_list : public
	choices <
		assignment_expression,
		regex<argument_expression_list, ',', assignment_expression>,
	> {};

class unary_expression : public
	choices <
		postfix_expression,
		regex<inc_op, unary_expression>,
		regex<dec_op, unary_expression>,
		regex<unary_operator, cast_expression>,
		regex<sizeof, unary_expression>,
		regex<sizeof, '(', type_name, ')'>,
	> {};

class unary_operator : public
	choices <
		'&',
		'*',
		'+',
		'-',
		'~',
		'!',
	> {};

class cast_expression : public
	choices <
		unary_expression,
		regex<'(', type_name, ')', cast_expression>,
	> {};

class multiplicative_expression : public
	choices <
		cast_expression,
		regex<multiplicative_expression, '*', cast_expression>,
		regex<multiplicative_expression, '/', cast_expression>,
		regex<multiplicative_expression, '%', cast_expression>,
	> {};

class additive_expression : public
	choices <
		multiplicative_expression,
		regex<additive_expression, '+', multiplicative_expression>,
		regex<additive_expression, '-', multiplicative_expression>,
	> {};

class shift_expression : public
	choices <
		additive_expression,
		regex<shift_expression, left_op, additive_expression>,
		regex<shift_expression, right_op, additive_expression>,
	> {};

class relational_expression : public
	choices <
		shift_expression,
		regex<relational_expression, '<', shift_expression>,
		regex<relational_expression, '>', shift_expression>,
		regex<relational_expression, le_op, shift_expression>,
		regex<relational_expression, ge_op, shift_expression>,
	> {};

class equality_expression : public
	choices <
		relational_expression,
		regex<equality_expression, eq_op, relational_expression>,
		regex<equality_expression, ne_op, relational_expression>,
	> {};

class and_expression : public
	choices <
		equality_expression,
		regex<and_expression, '&', equality_expression>,
	> {};

class exclusive_or_expression : public
	choices <
		and_expression,
		regex<exclusive_or_expression, '^', and_expression>,
	> {};

class inclusive_or_expression : public
	choices <
		exclusive_or_expression,
		regex<inclusive_or_expression, '|', exclusive_or_expression>,
	> {};

class logical_and_expression : public
	choices <
		inclusive_or_expression,
		regex<logical_and_expression, and_op, inclusive_or_expression>,
	> {};

class logical_or_expression : public
	choices <
		logical_and_expression,
		regex<logical_or_expression, or_op, logical_and_expression>,
	> {};

class conditional_expression : public
	choices <
		logical_or_expression,
		regex<logical_or_expression, '?', expression, ':', conditional_expression>,
	> {};

class assignment_expression : public
	choices <
		conditional_expression,
		regex<unary_expression, assignment_operator, assignment_expression>,
	> {};

class assignment_operator : public
	choices <
		'=',
		mul_assign,
		div_assign,
		mod_assign,
		add_assign,
		sub_assign,
		left_assign,
		right_assign,
		and_assign,
		xor_assign,
		or_assign,
	> {};

class expression : public
	choices <
		assignment_expression,
		regex<expression, ',', assignment_expression>,
	> {};

class constant_expression : public
	choices <
		conditional_expression,
	> {};

class declaration : public
	choices <
		regex<declaration_specifiers, ';'>,
		regex<declaration_specifiers, init_declarator_list, ';'>,
	> {};

class declaration_specifiers : public
	choices <
		storage_class_specifier,
		regex<storage_class_specifier, declaration_specifiers>,
		type_specifier,
		regex<type_specifier, declaration_specifiers>,
		type_qualifier,
		regex<type_qualifier, declaration_specifiers>,
	> {};

class init_declarator_list : public
	choices <
		init_declarator,
		regex<init_declarator_list, ',', init_declarator>,
	> {};

class init_declarator : public
	choices <
		declarator,
		regex<declarator, '=', initializer>,
	> {};

class storage_class_specifier : public
	choices <
		typedef,
		extern,
		static,
		auto,
		register,
	> {};

class type_specifier : public
	choices <
		void,
		char,
		short,
		int,
		long,
		float,
		double,
		signed,
		unsigned,
		struct_or_union_specifier,
		enum_specifier,
		type_name,
	> {};

class struct_or_union_specifier : public
	choices <
		regex<struct_or_union, identifier, '{', struct_declaration_list, '}'>,
		regex<struct_or_union, '{', struct_declaration_list, '}'>,
		regex<struct_or_union, identifier>,
	> {};

class struct_or_union : public
	choices <
		struct,
		union,
	> {};

class struct_declaration_list : public
	choices <
		struct_declaration,
		regex<struct_declaration_list, struct_declaration>,
	> {};

class struct_declaration : public
	choices <
		regex<specifier_qualifier_list, struct_declarator_list, ';'>,
	> {};

class specifier_qualifier_list : public
	choices <
		regex<type_specifier, specifier_qualifier_list>,
		type_specifier,
		regex<type_qualifier, specifier_qualifier_list>,
		type_qualifier,
	> {};

class struct_declarator_list : public
	choices <
		struct_declarator,
		regex<struct_declarator_list, ',', struct_declarator>,
	> {};

class struct_declarator : public
	choices <
		declarator,
		regex<':', constant_expression>,
		regex<declarator, ':', constant_expression>,
	> {};

class enum_specifier : public
	choices <
		regex<enum, '{', enumerator_list, '}'>,
		regex<enum, identifier, '{', enumerator_list, '}'>,
		regex<enum, identifier>,
	> {};

class enumerator_list : public
	choices <
		enumerator,
		regex<enumerator_list, ',', enumerator>,
	> {};

class enumerator : public
	choices <
		identifier,
		regex<identifier, '=', constant_expression>,
	> {};

class type_qualifier : public
	choices <
		const,
		volatile,
	> {};

class declarator : public
	choices <
		regex<pointer, direct_declarator>,
		direct_declarator,
	> {};

class direct_declarator : public
	choices <
		identifier,
		regex<'(', declarator, ')'>,
		regex<direct_declarator, '[', constant_expression, ']'>,
		regex<direct_declarator, '[', ']'>,
		regex<direct_declarator, '(', parameter_type_list, ')'>,
		regex<direct_declarator, '(', identifier_list, ')'>,
		regex<direct_declarator, '(', ')'>,
	> {};

class pointer : public
	choices <
		'*',
		regex<'*', type_qualifier_list>,
		regex<'*', pointer>,
		regex<'*', type_qualifier_list, pointer>,
	> {};

class type_qualifier_list : public
	choices <
		type_qualifier,
		regex<type_qualifier_list, type_qualifier>,
	> {};


class parameter_type_list : public
	choices <
		parameter_list,
		regex<parameter_list, ',', ellipsis>,
	> {};

class parameter_list : public
	choices <
		parameter_declaration,
		regex<parameter_list, ',', parameter_declaration>,
	> {};

class parameter_declaration : public
	choices <
		regex<declaration_specifiers, declarator>,
		regex<declaration_specifiers, abstract_declarator>,
		declaration_specifiers,
	> {};

class identifier_list : public
	choices <
		identifier,
		regex<identifier_list, ',', identifier>,
	> {};

class type_name : public
	choices <
		specifier_qualifier_list,
		regex<specifier_qualifier_list, abstract_declarator>,
	> {};

class abstract_declarator : public
	choices <
		pointer,
		direct_abstract_declarator,
		regex<pointer, direct_abstract_declarator>,
	> {};

class direct_abstract_declarator : public
	choices <
		regex<'(', abstract_declarator, ')'>,
		regex<'[', ']'>,
		regex<'[', constant_expression, ']'>,
		regex<direct_abstract_declarator, '[', ']'>,
		regex<direct_abstract_declarator, '[', constant_expression, ']'>,
		regex<'(', ')'>,
		regex<'(', parameter_type_list, ')'>,
		regex<direct_abstract_declarator, '(', ')'>,
		regex<direct_abstract_declarator, '(', parameter_type_list, ')'>,
	> {};

class initializer : public
	choices <
		assignment_expression,
		regex<'{', initializer_list, '}'>,
		regex<'{', initializer_list, ',', '}'>,
	> {};

class initializer_list : public
	choices <
		initializer,
		regex<initializer_list, ',', initializer>,
	> {};

class statement : public
	choices <
		labeled_statement,
		compound_statement,
		expression_statement,
		selection_statement,
		iteration_statement,
		jump_statement,
	> {};

class labeled_statement : public
	choices <
		regex<identifier, ':', statement>,
		regex<case, constant_expression, ':', statement>,
		regex<default, ':', statement>,
	> {};

class compound_statement : public
	choices <
		regex<'{', '}'>,
		regex<'{', statement_list, '}'>,
		regex<'{', declaration_list, '}'>,
		regex<'{', declaration_list, statement_list, '}'>,
	> {};

class declaration_list : public
	choices <
		declaration,
		regex<declaration_list, declaration>,
	> {};

class statement_list : public
	choices <
		statement,
		regex<statement_list, statement>,
	> {};

class expression_statement : public
	choices <
		';',
		regex<expression, ';'>,
	> {};

class selection_statement : public
	choices <
		regex<if, '(', expression, ')', statement>,
		regex<if, '(', expression, ')', statement, else, statement>,
		regex<switch, '(', expression, ')', statement>,
	> {};

class iteration_statement : public
	choices <
		regex<while, '(', expression, ')', statement>,
		regex<do, statement, while, '(', expression, ')', ';'>,
		regex<for, '(', expression_statement, expression_statement, ')', statement>,
		regex<for, '(', expression_statement, expression_statement, expression, ')', statement>,
	> {};

class jump_statement : public
	choices <
		regex<goto, identifier, ';'>,
		regex<continue, ';'>,
		regex<break, ';'>,
		regex<return, ';'>,
		regex<return, expression, ';'>,
	> {};

class translation_unit : public
	choices <
		external_declaration,
		regex<translation_unit, external_declaration>,
	> {};

class external_declaration : public
	choices <
		function_definition,
		declaration,
	> {};

class function_definition : public
	choices <
		regex<declaration_specifiers, declarator, declaration_list, compound_statement>,
		regex<declaration_specifiers, declarator, compound_statement>,
		regex<declarator, declaration_list, compound_statement>,
		regex<declarator, compound_statement>,
	> {};

#endif // C_GRAMMAR_H

