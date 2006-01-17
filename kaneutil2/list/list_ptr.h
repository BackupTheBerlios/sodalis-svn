#ifndef KU__LIST_PTR_H__
#define KU__LIST_PTR_H__
#include "errors/open_code.h"

#define LIST_TYPE void *
#define LIST_EQ(a, b) (a==b)
#define LIST_ERRVAL NULL
#define LIST_ID ptr
#include "list_core.h"

#include "errors/close_code.h"
#endif
