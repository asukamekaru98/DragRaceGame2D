#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/title_resource.h"
#include "../header/opening.h"
#include "../header/title.h"

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
    DrawExtendGraph(0, 0, 800, 600, g_hBg, TRUE);

    // Layer 2: shutter (static during opening)
    DrawExtendGraph(0, 0, 800, 600, g_hShutter, TRUE);

    // Layer 3: logo (fade in)
    int logoW = 0, logoH = 0;
    GetGraphSize(g_hLogo, &logoW, &logoH);
    int logoX = (800 - logoW) / 2;
    int logoY = (600 - logoH) / 2;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, s_logoAlpha);
    DrawGraph(logoX, logoY, g_hLogo, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
