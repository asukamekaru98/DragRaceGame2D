#include "../DxLib/DxLib.h"
#include "share.h"
#include "screen_manager.h"
#include "input.h"
#include "game_screen.h"
#include "game_data.h"
#include "result.h"

#define WAIT_FRAMES 60   // 60fps * 1sec

static int s_timer = 0;

void UpdateGame(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        s_timer = 0;
        s_initialized = 1;
    }

    s_timer++;
    if (s_timer >= WAIT_FRAMES) {
        // Dummy result data for now
        g_gameData.lastRank = 1;
        g_gameData.lastTime = 12.34f;

        s_initialized = 0;
        ChangeScreen(UpdateResult, DrawResult);
    }
}

void DrawGame(void) {
    DrawFillBox(0, 0, 800, 600, Color(10, 10, 20).Code());
    DrawString(300, 270, "RACE IN PROGRESS...", Color(200, 200, 200).Code());
    DrawFormatString(340, 310, Color(100, 100, 100).Code(), "%d / %d", s_timer, WAIT_FRAMES);
}
