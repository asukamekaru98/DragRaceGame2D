#include "../DxLib/DxLib.h"
#include "screen_manager.h"
#include "opening.h"
#include "title.h"
#include "garage.h"
#include "dealer.h"
#include "customize.h"
#include "game_screen.h"
#include "result.h"
#include "debug.h"

ScreenManager g_screen = { NULL, NULL, NULL, NULL, NULL, NULL };

// Function table indexed by SCREEN_NAME.
// updateInput is NULL for screens that take no key input.
SCREEN_FUNCTIONS g_ScreenFuncs[SCREEN_COUNT] = {
	{ NULL,                 UpdateOpening,   DrawOpening   },
	{ UpdateTitleInput,     UpdateTitle,     DrawTitle     },
	{ UpdateGarageInput,    UpdateGarage,    DrawGarage    },
	{ UpdateDealerInput,    UpdateDealer,    DrawDealer    },
	{ UpdateCustomizeInput, UpdateCustomize, DrawCustomize },
	{ NULL,                 UpdateGame,      DrawGame      },
	{ UpdateResultInput,    UpdateResult,    DrawResult    },
#ifdef _DEBUG
	{ UpdateDebugRawInput,   UpdateDebugMenu,  DrawDebugMenu  },
	{ UpdateDebugRawInput,   UpdateDebugDraw,  DrawDebugDraw  },
	{ UpdateDebugRawInput,   UpdateDebugInput, DrawDebugInput },
	{ UpdateDebugMeterInput, UpdateDebugMeter, DrawDebugMeter },
#endif
};

void ChangeScreen(UpdateInputFunc updateInput, UpdateFunc update, DrawFunc draw) {
    g_screen.nextUpdateInput = updateInput;
    g_screen.nextUpdate      = update;
    g_screen.nextDraw        = draw;
}
