#pragma once
#include "typedef.h"

typedef enum {
    PARTS_CAT_ENGINE   = 0,
    PARTS_CAT_TIRE     = 1,
    PARTS_CAT_BODY     = 2,
    PARTS_CAT_EXTERIOR = 3,
    PARTS_CAT_COUNT    = 4,
} PARTS_CATEGORY;

typedef struct {
    const char*    name;
    PARTS_CATEGORY category;
    SI_4           price;
    F_4            maxSpeedBonus;
    F_4            accelBonus;
} PartsData;

extern const PartsData PARTS_TABLE[];
extern const SI_4      PARTS_TABLE_COUNT;
