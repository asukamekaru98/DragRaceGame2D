#include "../DxLib/DxLib.h"
#include "../header/share.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/title_resource.h"
#include "../header/title.h"
#include "../header/garage.h"
#include "../header/debug.h"

#define SHUTTER_SPEED      8.0f
#define BLINK_INTERVAL     30

// Sprite cells inside tite_sprite2.png (g_hShutter)
#define GARAGE_SRC_X       0
#define GARAGE_SRC_Y       0
#define GARAGE_WIDTH       674
#define GARAGE_HEIGHT      376
#define OP_SHUTTER_SRC_X      0
#define OP_SHUTTER_SRC_Y      378
#define OP_SHUTTER_SRC_WIDTH      674
#define OP_SHUTTER_SRC_HEIGHT     378

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

    // Layer 1 (back): garage interior - sprite cell of tite_sprite2
   //DrawRectGraph(0, 0,
   //    GARAGE_SRC_X, GARAGE_SRC_Y, GARAGE_WIDTH, GARAGE_HEIGHT,
   //    g_hShutter, TRUE, FALSE);

    // Layer 2 (middle): shutter - sprite cell of tite_sprite2 (rises upward in SHUTTER_UP state)
   //DrawRectGraph(0, (int)s_shutterY,
   //    SHUTTER_SRC_X, SHUTTER_SRC_Y, SHUTTER_WIDTH, SHUTTER_HEIGHT,
   //    g_hShutter, TRUE, FALSE);

    // Layer 3 (front): full title image - tite_sprite1
    //DrawGraph(0, 0, g_hBg, TRUE);
    DrawRectGraph(0, 0, 0, 0, 800, 600, g_hBg, TRUE, FALSE);

    // Layer 4: logo (fully opaque)
    DrawGraph(logoX, logoY, g_hLogo, TRUE);

    // "PRESS ANY KEY" blinking (WAIT state only)
    if (s_state == TITLE_STATE_WAIT) {
        int visible = (s_blinkTimer / BLINK_INTERVAL) % 2;
        if (visible) {
            DrawString(284, 500, "PRESS ANY KEY", Color::WHITE.Code());
        }
    }

#ifdef _DEBUG
    DrawString(630, 570, "[F1] DXLIB SAMPLE", Color::YELLOW.Code());
#endif
}
