#include "../header/parts_data.h"

const PartsData PARTS_TABLE[] = {
    // Engine
    { "Sports Engine A", PARTS_CAT_ENGINE,    500,  10.0f, 0.2f },
    { "Turbo Engine B",  PARTS_CAT_ENGINE,   1200,  30.0f, 0.5f },
    { "Race Engine C",   PARTS_CAT_ENGINE,   3000,  60.0f, 1.0f },
    // Tire
    { "Sports Tire A",   PARTS_CAT_TIRE,      400,   0.0f, 0.3f },
    { "Racing Tire B",   PARTS_CAT_TIRE,     1000,   5.0f, 0.8f },
    // Body
    { "Light Body A",    PARTS_CAT_BODY,      600,  10.0f, 0.4f },
    { "Aero Body B",     PARTS_CAT_BODY,     1800,  20.0f, 0.6f },
    // Exterior
    { "Custom Paint A",  PARTS_CAT_EXTERIOR,  300,   0.0f, 0.0f },
    { "Race Livery B",   PARTS_CAT_EXTERIOR,  800,   0.0f, 0.0f },
};
const SI_4 PARTS_TABLE_COUNT = (SI_4)(sizeof(PARTS_TABLE) / sizeof(PARTS_TABLE[0]));
