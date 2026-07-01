#pragma once
//#include "typedef.h"

typedef enum {
    PARTS_CAT_ENGINE   = 0,
    PARTS_CAT_TIRE     = 1,
    PARTS_CAT_BODY     = 2,
    PARTS_CAT_EXTERIOR = 3,
    PARTS_CAT_COUNT    = 4,
} PARTS_CATEGORY;

typedef enum {
    GAUGE_NONE,
    GAUGE_TURBO,
    GAUGE_OIL_TEMP,
}GAUGE_TYPE;

typedef struct {
    const char*     pName;
    PARTS_CATEGORY  eCategory;
    GAUGE_TYPE      eGaugeType;
    int             iPrice;
    float           fMaxSpeedBonus;
    float           fAccelBonus;
} PartsData;

extern const PartsData  PARTS_TABLE[];
extern const int        PARTS_TABLE_COUNT;
