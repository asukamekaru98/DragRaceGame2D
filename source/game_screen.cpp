#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/game_screen.h"
#include "../header/game_data.h"
#include "../header/result.h"

#define WAIT_FRAMES 60   // 60fps * 1sec

static SI_4 s_timer = 0;

void UpdateGame(void) {
    static SI_4 s_initialized = 0;
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
    DrawFillBox(0, 0, 800, 600, GetColor(10, 10, 20));
    DrawString(300, 270, "RACE IN PROGRESS...", GetColor(200, 200, 200));
    DrawFormatString(340, 310, GetColor(100, 100, 100), "%d / %d", s_timer, WAIT_FRAMES);
}
