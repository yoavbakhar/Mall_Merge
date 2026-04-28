#include "item.h"

int ItemIsEmpty(const Item *item) {
    return item->type == 0;
}
