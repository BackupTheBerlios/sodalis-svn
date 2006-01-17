#define ABTREE_TYPE cfg_query_t*
#define ABTREE_EQ(a, b) (strcmp(a->id,b->id)==0)
#define ABTREE_MORE(a, b) (strcmp(a->id,b->id)>0)
#define ABTREE_ERRVAL NULL
#define ABTREE_ID cfg
#include "abtree/abtree_core.h"
