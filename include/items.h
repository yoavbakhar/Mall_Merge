#ifndef ITEMS_H
#define ITEMS_H

#include "raylib.h"

/* Item IDs - unique identifiers for each item in the game */
typedef enum {
    ITEM_ID_EMPTY = 0,
    
    /* Chain A: Janitor's Secret (Generator: Lost & Found Box) */
    ITEM_ID_GENERATOR_LOST_FOUND = 100,  /* Generator for Chain A */
    ITEM_ID_FADED_RECEIPT,               /* Level 1 */
    ITEM_ID_CRINKLED_MAP,                /* Level 2 */
    ITEM_ID_PLASTIC_BADGE,               /* Level 3 */
    ITEM_ID_HEAVY_FLASHLIGHT,            /* Level 4 */
    ITEM_ID_SKELETON_KEY,                /* Level 5 */
    
    /* Chain B: Thread-Bare Boutique (Generator: Rusted Sewing Kit) */
    ITEM_ID_GENERATOR_SEWING_KIT = 200,  /* Generator for Chain B */
    ITEM_ID_LOOSE_BUTTON,                /* Level 1 */
    ITEM_ID_SAFETY_PIN,                  /* Level 2 */
    ITEM_ID_NEON_THREAD,                 /* Level 3 */
    ITEM_ID_VINTAGE_SCISSORS,            /* Level 4 */
    ITEM_ID_DESIGNERS_MANNEQUIN,         /* Level 5 */
    
    /* Chain C: Neon Arcade (no generator - obtained via task) */
    ITEM_ID_NEON_TOKEN = 300,            /* Level 1 */
    ITEM_ID_JOYSTICK,                    /* Level 2 */
    ITEM_ID_GAME_CARTRIDGE,              /* Level 3 */
    ITEM_ID_ARCADE_CABINET,              /* Level 4 */
    ITEM_ID_HIGH_SCORE_TROPHY,           /* Level 5 - Final Room Restoration Item */
    
    ITEM_ID_COUNT
} ItemID;

/* Chain IDs - which family an item belongs to */
typedef enum {
    CHAIN_EMPTY = 0,
    CHAIN_JANITORS_SECRET = 1,
    CHAIN_THREAD_BARE = 2,
    CHAIN_NEON_ARCADE = 3,
} ChainID;

/* Item definition - metadata for each item type */
typedef struct {
    ItemID id;
    ChainID chain_id;
    int level;                    /* 1-5, or 0 for generators */
    int is_generator;             /* 1 if this is a generator, 0 otherwise */
    const char *name;
    const char *asset_path;       /* PNG file path for this item */
    Color placeholder_color;      /* Fallback color if texture fails to load */
} ItemDefinition;

/* Get item definition by ItemID */
const ItemDefinition *GetItemDefinition(ItemID id);

/* Get next item in chain after merge (or NULL if already max level) */
ItemID GetNextItemInChain(ItemID current_id);

/* Get the ItemID for level 1 of a chain (what generators spawn) */
ItemID GetChainLevel1Item(ChainID chain_id);

/* Get the generator ItemID for a chain */
ItemID GetChainGenerator(ChainID chain_id);

#endif