#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include <stdio.h>
#include "reader.h"
#include "tree.h"
#include <stdlib.h>

node_t* g_get_go(list_t *lst);
node_t* g_get_e();
node_t* g_get_ram();
node_t* g_get_num();
node_t* g_get_line();

#endif // _GRAMMAR_H_
