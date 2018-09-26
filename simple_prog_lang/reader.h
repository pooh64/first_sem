#ifndef _READER_H_
#define _READER_H_

#include "tree.h"
typedef struct list_t list_t;

struct list_t
{
    node_data_t data;
    node_type_t type;
    list_t *next;
};

list_t* r_go(char *str);
int list_destruct_rec(list_t *list);

#endif // _READER_H_
