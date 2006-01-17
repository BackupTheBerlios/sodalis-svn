#ifndef KU__ABTREE_INT_H__
#define KU__ABTREE_INT_H__
#include "errors/open_code.h"

#define ABTREE_TYPE int
#define ABTREE_EQ(a, b) (a==b)
#define ABTREE_MORE(a, b) (a>b)
#define ABTREE_ERRVAL -1
#define ABTREE_ID int
#include "abtree_core.h"

#include "errors/close_code.h"
#endif
