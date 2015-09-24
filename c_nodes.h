#ifndef C_NODES_H
#define C_NODES_H

#include "token.h"

struct node_base
{

};

template<token_type Tok>
struct token_node : node_base
{

};

#endif // C_NODES_H
