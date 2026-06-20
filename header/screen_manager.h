#pragma once

typedef void (*UpdateFunc)(void);
typedef void (*DrawFunc)(void);

typedef struct {
    UpdateFunc update;
    DrawFunc   draw;
    UpdateFunc nextUpdate;
    DrawFunc   nextDraw;
} ScreenManager;

extern ScreenManager g_screen;

void ChangeScreen(UpdateFunc update, DrawFunc draw);
