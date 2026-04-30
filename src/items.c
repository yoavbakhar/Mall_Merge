#include "items.h"

/* Item definition lookup table */
static const ItemDefinition item_definitions[] = {
    /* Empty item */
    {ITEM_ID_EMPTY, CHAIN_EMPTY, 0, 0, "Empty", NULL, {0, 0, 0, 0}},
    
    /* Chain A: Janitor's Secret */
    {ITEM_ID_GENERATOR_LOST_FOUND, CHAIN_JANITORS_SECRET, 0, 1, 
     "Lost & Found Box", "assets/Lost_And_Found.png", {200, 180, 100, 255}},
    
    {ITEM_ID_FADED_RECEIPT, CHAIN_JANITORS_SECRET, 1, 0,
     "Faded Receipt", "assets/chain_0_0.png", {180, 160, 100, 255}},
    
    {ITEM_ID_CRINKLED_MAP, CHAIN_JANITORS_SECRET, 2, 0,
     "Crinkled Mall Map", "assets/chain_0_1.png", {170, 150, 90, 255}},
    
    {ITEM_ID_PLASTIC_BADGE, CHAIN_JANITORS_SECRET, 3, 0,
     "Plastic Badge Holder", "assets/chain_0_2.png", {160, 140, 80, 255}},
    
    {ITEM_ID_HEAVY_FLASHLIGHT, CHAIN_JANITORS_SECRET, 4, 0,
     "Heavy Flashlight", "assets/chain_0_3.png", {150, 130, 70, 255}},
    
    {ITEM_ID_SKELETON_KEY, CHAIN_JANITORS_SECRET, 5, 0,
     "The Skeleton Key", "assets/chain_0_4.png", {200, 180, 60, 255}},
    
    /* Chain B: Thread-Bare Boutique */
    {ITEM_ID_GENERATOR_SEWING_KIT, CHAIN_THREAD_BARE, 0, 1,
     "Rusted Sewing Kit", "assets/Sewing_Kit.png", {180, 140, 150, 255}},
    
    {ITEM_ID_LOOSE_BUTTON, CHAIN_THREAD_BARE, 1, 0,
     "Loose Button", "assets/chain_1_0 (1).png", {150, 120, 140, 255}},
    
    {ITEM_ID_SAFETY_PIN, CHAIN_THREAD_BARE, 2, 0,
     "Safety Pin Link", "assets/chain_1_1.png", {140, 110, 130, 255}},
    
    {ITEM_ID_NEON_THREAD, CHAIN_THREAD_BARE, 3, 0,
     "Spool of Neon Thread", "assets/chain_1_2.png", {200, 100, 200, 255}},
    
    {ITEM_ID_VINTAGE_SCISSORS, CHAIN_THREAD_BARE, 4, 0,
     "Vintage Fabric Scissors", "assets/chain_1_3.png", {160, 100, 150, 255}},
    
    {ITEM_ID_DESIGNERS_MANNEQUIN, CHAIN_THREAD_BARE, 5, 0,
     "The Designer's Mannequin", "assets/chain_1_4.png", {200, 120, 180, 255}},
};

#define ITEM_DEFINITIONS_COUNT (sizeof(item_definitions) / sizeof(ItemDefinition))

const ItemDefinition *GetItemDefinition(ItemID id) {
    /* Search through the definitions for matching ID */
    for (int i = 0; i < ITEM_DEFINITIONS_COUNT; i++) {
        if (item_definitions[i].id == id) {
            return &item_definitions[i];
        }
    }
    
    /* Not found, return empty item definition */
    return &item_definitions[0];
}

ItemID GetNextItemInChain(ItemID current_id) {
    const ItemDefinition *current = GetItemDefinition(current_id);
    
    /* Generators and max level items don't merge further */
    if (current->is_generator || current->level >= 5) {
        return ITEM_ID_EMPTY;
    }
    
    int next_level = current->level + 1;
    
    /* Find item in same chain with next level */
    for (int i = 0; i < ITEM_DEFINITIONS_COUNT; i++) {
        if (item_definitions[i].chain_id == current->chain_id && 
            item_definitions[i].level == next_level &&
            !item_definitions[i].is_generator) {
            return item_definitions[i].id;
        }
    }
    
    return ITEM_ID_EMPTY;  /* Not found */
}

ItemID GetChainLevel1Item(ChainID chain_id) {
    for (int i = 0; i < ITEM_DEFINITIONS_COUNT; i++) {
        if (item_definitions[i].chain_id == chain_id && 
            item_definitions[i].level == 1 &&
            !item_definitions[i].is_generator) {
            return item_definitions[i].id;
        }
    }
    return ITEM_ID_EMPTY;
}

ItemID GetChainGenerator(ChainID chain_id) {
    for (int i = 0; i < ITEM_DEFINITIONS_COUNT; i++) {
        if (item_definitions[i].chain_id == chain_id && 
            item_definitions[i].is_generator) {
            return item_definitions[i].id;
        }
    }
    return ITEM_ID_EMPTY;
}
