#ifndef ITEM_H
#define ITEM_H

#include "items.h"

typedef struct {
    ItemID item_id;       /* Which item this is (from items.h) */
    int is_generator;     /* Quick flag for generators */
} Item;

/* Check if item slot is empty */
int ItemIsEmpty(const Item *item);

#endif
