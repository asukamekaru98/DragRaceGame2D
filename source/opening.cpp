#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/title_resource.h"
#include "../header/opening.h"
#include "../header/title.h"
#include "../header/const.h"

#define LOGO_FADE_SPEED  2

static int s_logoAlpha = 0;

static void ResetOpening(void) {
    s_logoAlpha = 0;
}

void UpdateOpening(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        ResetOpening();
        s_initialized = 1;
    }


    s_logoAlpha += LOGO_FADE_SPEED;
    if (s_logoAlpha >= 255) {
        s_logoAlpha   = 255;
        s_initialized = 0;
        ChangeScreen(UpdateTitle, DrawTitle);
    }
}


void DrawOpening(void) {
    // Layer 1: garage background
    DrawRectGraph(
        OP_SHUTTER_X, OP_SHUTTER_Y,
        OP_SHUTTER_SRC_X, OP_SHUTTER_SRC_Y, OP_SHUTTER_SRC_WIDTH, OP_SHUTTER_SRC_HEIGHT,
        g_hShutter, TRUE, FALSE
    );

    // Layer 2: shutter (static during opening)
    DrawRectGraph(
        OP_BG_X, OP_BG_Y, 
        OP_BG_SRC_X, OP_BG_SRC_Y, OP_BG_SRC_WIDTH, OP_BG_SRC_HEIGHT, 
        g_hBg, TRUE, FALSE
    );

    // Layer 3: logo (fade in)
    int logoW = 0, logoH = 0;
    GetGraphSize(g_hLogo, &logoW, &logoH);
    int logoX = (800 - logoW) / 2;
    int logoY = (600 - logoH) / 2;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, s_logoAlpha);
    DrawGraph(logoX, logoY, g_hLogo, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
