#define ABTREE_TYPE restype_t*
#define ABTREE_EQ(a, b) (a->type==b->type)
#define ABTREE_MORE(a, b) (a->type>b->type)
#define ABTREE_ERRVAL NULL
#define ABTREE_ID restype
#include "abtree/abtree_core.h"
