#pragma once

typedef void (*UpdateInputFunc)(void);
typedef void (*UpdateFunc)(void);
typedef void (*DrawFunc)(void);

typedef struct {
	UpdateInputFunc updateInput;
    UpdateFunc update;
    DrawFunc   draw;
	UpdateInputFunc nextUpdateInput;
    UpdateFunc nextUpdate;
    DrawFunc   nextDraw;
} ScreenManager;

extern ScreenManager g_screen;

typedef enum {
	SCREEN_OPENING,
	SCREEN_TITLE,
	SCREEN_GARAGE,
	/* Debugån */
	SCREEN_DEBUG_MENU,
	SCREEN_DEBUG_DRAW,
	SCREEN_DEBUG_INPUT,
	SCREEN_DEBUG_METER,
	SCREEN_COUNT,			// óvëfêî
}SCREEN_NAME;

typedef struct {
	UpdateInputFunc updateInput;
	UpdateFunc update;
	DrawFunc   draw;
}SCREEN_FUNCTIONS;

extern SCREEN_FUNCTIONS g_ScreenFuncs[][3] = {
	{ NULL, UpdateOpening, DrawOpening },
	{ NULL, UpdateDebugMenu, DrawDebugMenu },

	{ NULL, UpdateDebugDraw, DrawDebugDraw },
	{ NULL, UpdateDebugInput, DrawDebugInput },
	{ NULL, UpdateDebugMeter, DrawDebugMeter},
	{ NULL, UpdateDebugMeter, DrawDebugMeter},
	{ NULL, UpdateDebugMeter, DrawDebugMeter},
};

void ChangeScreen(UpdateFunc update, DrawFunc draw);
