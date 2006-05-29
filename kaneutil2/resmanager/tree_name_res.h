#define ABTREE_TYPE res_t*
#define ABTREE_EQ(a, b) (strcmp(a->name,b->name)==0)
#define ABTREE_MORE(a, b) (strcmp(a->name,b->name)>0)
#define ABTREE_ERRVAL NULL
#define ABTREE_ID res
#include "abtree/abtree_core.h"
