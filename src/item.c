#include "item.h"

int ItemIsEmpty(const Item *item) {
    return item->item_id == ITEM_ID_EMPTY;
}
