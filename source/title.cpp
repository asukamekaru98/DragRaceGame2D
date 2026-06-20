#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/title_resource.h"
#include "../header/title.h"
#include "../header/garage.h"
#include "../header/debug.h"

#define SHUTTER_SPEED      8.0f
#define BLINK_INTERVAL     30

typedef enum {
    TITLE_STATE_WAIT,
    TITLE_STATE_SHUTTER_UP,
} TITLE_STATE;

static TITLE_STATE s_state      = TITLE_STATE_WAIT;
static float       s_shutterY   = 0.0f;
static int         s_blinkTimer = 0;

static void ResetTitle(void) {
    s_state      = TITLE_STATE_WAIT;
    s_shutterY   = 0.0f;
    s_blinkTimer = 0;
}

void UpdateTitle(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        ResetTitle();
        s_initialized = 1;
    }

    switch (s_state) {

    case TITLE_STATE_WAIT:
        s_blinkTimer++;

#ifdef _DEBUG
        if (IsKeyTriggered(KEY_INPUT_F1)) {
            s_initialized = 0;
            ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
            return;
        }
#endif
        if (CheckHitKeyAll(DX_CHECKINPUT_ALL) != 0) {
            s_state = TITLE_STATE_SHUTTER_UP;
        }
        break;

    case TITLE_STATE_SHUTTER_UP:
        s_shutterY -= SHUTTER_SPEED;
        if (s_shutterY <= -600.0f) {
            s_initialized = 0;
            ChangeScreen(UpdateGarage, DrawGarage);
        }
        break;
    }
}

void DrawTitle(void) {
    int logoW = 0, logoH = 0;
    GetGraphSize(g_hLogo, &logoW, &logoH);
    int logoX = (800 - logoW) / 2;
    int logoY = (600 - logoH) / 2;

    // Layer 1: garage background
    DrawExtendGraph(0, 0, 800, 600, g_hBg, TRUE);

    // Layer 2: shutter (rises upward in SHUTTER_UP state)
    DrawExtendGraph(0, (int)s_shutterY, 800, (int)s_shutterY + 600, g_hShutter, TRUE);

    // Layer 3: logo (fully opaque)
    DrawGraph(logoX, logoY, g_hLogo, TRUE);

    // "PRESS ANY KEY" blinking (WAIT state only)
    if (s_state == TITLE_STATE_WAIT) {
        int visible = (s_blinkTimer / BLINK_INTERVAL) % 2;
        if (visible) {
            DrawString(284, 500, "PRESS ANY KEY", GetColor(255, 255, 255));
        }
    }

#ifdef _DEBUG
    DrawString(630, 570, "[F1] DXLIB SAMPLE", GetColor(255, 255, 0));
#endif
}
