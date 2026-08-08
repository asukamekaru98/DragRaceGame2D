#pragma once


//typedef struct {
//	UpdateInputFunc updateInput;
//	UpdateFunc update;
//	DrawFunc   draw;
//	UpdateInputFunc nextUpdateInput;
//	UpdateFunc nextUpdate;
//	DrawFunc   nextDraw;
//} ScreenManager;

//extern ScreenManager g_screen;

typedef struct {
	ScreenManager* screenManager;
	ScreenManager* nextScreenManager;
} ScreenManagerEx;

extern ScreenManagerEx g_screenEx;

typedef enum {
	SCREEN_OPENING,
	SCREEN_TITLE,
	SCREEN_GARAGE,
	SCREEN_DEALER,
	SCREEN_CUSTOMIZE,
	SCREEN_GAME,
	SCREEN_RESULT,
#ifdef _DEBUG
	SCREEN_DEBUG_MENU,
	SCREEN_DEBUG_DRAW,
	SCREEN_DEBUG_INPUT,
	SCREEN_DEBUG_METER,
#endif
	SCREEN_COUNT,			// number of screens
} SCREEN_NAME;

typedef struct {
	UpdateInputFunc updateInput;
	UpdateFunc update;
	DrawFunc   draw;
} SCREEN_FUNCTIONS;

extern SCREEN_FUNCTIONS g_ScreenFuncs[SCREEN_COUNT];

void ChangeScreen(UpdateInputFunc updateInput, UpdateFunc update, DrawFunc draw);


class ScreenManager {
protected:
	void UpdateInputFunc(void);
	void UpdateFunc(void);
	void DrawFunc(void);

public:

};