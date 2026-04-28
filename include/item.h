#ifndef ITEM_H
#define ITEM_H

#include "constants.h"

typedef struct {
    int type;           /* ITEM_TYPE_NORMAL, ITEM_TYPE_GENERATOR, 0=empty */
    int level;          /* Item level (1=base, higher=merged) */
    int is_generator;   /* 1 if this is a generator, 0 otherwise */
} Item;

/* Check if item slot is empty */
int ItemIsEmpty(const Item *item);

#endif
