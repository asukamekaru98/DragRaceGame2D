#include "../DxLib/DxLib.h"
#include "screen_manager.h"

ScreenManager g_screen = { NULL, NULL, NULL, NULL };

void ChangeScreen(UpdateFunc update, DrawFunc draw) {
    g_screen.nextUpdate = update;
    g_screen.nextDraw   = draw;
}
