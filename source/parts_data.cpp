#include "parts_data.h"

const PartsData PARTS_TABLE[] = {
    // Engine
	{ "Sports Engine A", PARTS_CAT_ENGINE,	GAUGE_NONE,500,  10.0f, 0.2f },
	{ "Turbo Engine B",  PARTS_CAT_ENGINE,	GAUGE_NONE,  1200,  30.0f, 0.5f },
	{ "Race Engine C",   PARTS_CAT_ENGINE,	GAUGE_NONE,  3000,  60.0f, 1.0f },
	// Tire
	{ "Sports Tire A",   PARTS_CAT_TIRE,	GAUGE_NONE,   400,   0.0f, 0.3f },
	{ "Racing Tire B",   PARTS_CAT_TIRE,	GAUGE_NONE,   1000,   5.0f, 0.8f },
	// Body
	{ "Light Body A",    PARTS_CAT_BODY,	GAUGE_NONE,    600,  10.0f, 0.4f },
	{ "Aero Body B",     PARTS_CAT_BODY,	GAUGE_NONE,   1800,  20.0f, 0.6f },
	// Exterior
	{ "Custom Paint A",  PARTS_CAT_EXTERIOR,	GAUGE_NONE, 300,   0.0f, 0.0f },
	{ "Race Livery B",   PARTS_CAT_EXTERIOR,	GAUGE_NONE, 800,   0.0f, 0.0f },
};
const int PARTS_TABLE_COUNT = (int)(sizeof(PARTS_TABLE) / sizeof(PARTS_TABLE[0]));
