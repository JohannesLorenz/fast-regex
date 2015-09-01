#include "token.h"

token_t::token_t()
{
}


#define K_TOKEN(x) tokens["x"] = k_##x

void token_map::init_tokens()
{
	//tokens[identifier] = "<identifier>";
	//tokens[k_auto] = "auto";
	//tokens[k_break] = "break";
	//tokens[k_case] =
	K_TOKEN(auto);
	K_TOKEN(break);
	K_TOKEN(case);
	K_TOKEN(char);
	K_TOKEN(const);
	K_TOKEN(continue);
	K_TOKEN(default);
	K_TOKEN(do);
	K_TOKEN(double);
	K_TOKEN(else);
	K_TOKEN(enum);
	K_TOKEN(extern);
	K_TOKEN(float);
	K_TOKEN(for);
	K_TOKEN(goto);
	K_TOKEN(if);
	K_TOKEN(int);
	K_TOKEN(long);
	K_TOKEN(register);
	K_TOKEN(return);
	K_TOKEN(short);
	K_TOKEN(signed);
	K_TOKEN(sizeof);
	K_TOKEN(static);
	K_TOKEN(struct);
	K_TOKEN(switch);
	K_TOKEN(typedef);
	K_TOKEN(union);
	K_TOKEN(unsigned);
	K_TOKEN(void);
	K_TOKEN(volatile);
	K_TOKEN(while);

	tokens["..."] = ellipsis;
	tokens[">>="] = right_assign;
	tokens["<<="] = left_assign;
	tokens["+="] = add_assign;
	tokens["-="] = sub_assign;
	tokens["*="] = mul_assign;
	tokens["/="] = div_assign;
	tokens["%="] = mod_assign;
	tokens["&="] = and_assign;
	tokens["^="] = xor_assign;
	tokens["|="] = or_assign;
	tokens[">>"] = right_op;
	tokens["<<"] = left_op;
	tokens["++"] = inc_op;

}

#undef K_TOKEN

token_map::token_map()
{
	init_tokens();
}
