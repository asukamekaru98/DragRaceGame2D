#include "../DxLib/DxLib.h"
#include "share.h"
#include "screen_manager.h"
#include "input/result_input.h"
#include "game_data.h"
#include "result.h"
#include "garage.h"

// ── Constants ────────────────────────────────────────────────
#define RESULT_ITEM_COUNT   5
#define FADE_SPEED          5.0f
#define ITEM_WAIT_FRAMES    20
#define BLINK_INTERVAL      30

#define RESULT_LABEL_X      180
#define RESULT_Y            240
#define RESULT_LINE_H        50

#define PRIZE_WIN_BASE       1000
#define PRIZE_TIME_BONUS_BASE 5000
#define PRIZE_LOSE           200

// ── State ────────────────────────────────────────────────────
static float s_itemAlpha[RESULT_ITEM_COUNT];
static int s_currentItem;
static int s_waitTimer;
static int s_allShown;
static int s_blinkTimer;
static int s_isBestUpdated;
static int s_prize;
static int s_hBg = -1;
static ResultInput s_input;

// ── Helpers ──────────────────────────────────────────────────
static int CalcPrize(int rank, float raceTime) {
    if (rank == 1) {
        int bonus = (raceTime > 0.0f) ? (int)(PRIZE_TIME_BONUS_BASE / raceTime) : 0;
        return PRIZE_WIN_BASE + bonus;
    }
    return PRIZE_LOSE;
}

static void FormatTime(float t, int* min, int* sec, int* msec) {
    *min  = (int)(t / 60.0f);
    *sec  = (int)(t) % 60;
    *msec = (int)((t - (int)t) * 100);
}

// ── Fade update ───────────────────────────────────────────────
static void UpdateResultFade(void) {
    if (s_currentItem >= RESULT_ITEM_COUNT) {
        s_allShown = 1;
        return;
    }

    s_itemAlpha[s_currentItem] += FADE_SPEED;

    if (s_itemAlpha[s_currentItem] >= 255.0f) {
        s_itemAlpha[s_currentItem] = 255.0f;
        s_waitTimer++;
        if (s_waitTimer >= ITEM_WAIT_FRAMES) {
            s_currentItem++;
            s_waitTimer = 0;
        }
    }
}

// ── Screen functions ──────────────────────────────────────────
void UpdateResultInput(void) {
    s_input.Update();
}

void UpdateResult(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        for (int i = 0; i < RESULT_ITEM_COUNT; i++) s_itemAlpha[i] = 0.0f;
        s_currentItem   = 0;
        s_waitTimer     = 0;
        s_allShown      = 0;
        s_blinkTimer    = 0;
        s_isBestUpdated = 0;

        s_prize = CalcPrize(g_gameData.lastRank, g_gameData.lastTime);
        g_gameData.money += s_prize;

        if (g_gameData.lastRank == 1) {
            if (g_gameData.bestTime < 0.0f || g_gameData.lastTime < g_gameData.bestTime) {
                g_gameData.bestTime = g_gameData.lastTime;
                s_isBestUpdated = 1;
            }
        }

        s_hBg = LoadGraph("resource/result/bg_result.png");
        s_input.Update();   // re-sync: suppress false triggers from keys held across screens
        s_initialized = 1;
    }

    UpdateResultFade();

    if (s_allShown) {
        s_blinkTimer++;

        if (s_input.IsAnyInputPressed()) {
            if (s_hBg >= 0) { DeleteGraph(s_hBg); s_hBg = -1; }
            s_initialized = 0;
            ChangeScreen(UpdateGarageInput, UpdateGarage, DrawGarage);
        }
    }
}

void DrawResult(void) {
    // Background
    if (s_hBg >= 0) {
        DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);
    } else {
        DrawFillBox(0, 0, 800, 600, Color(15, 10, 25).Code());
    }

    // ① Win / Lose
    if (s_itemAlpha[0] > 0.0f) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[0]);
        if (g_gameData.lastRank == 1) {
            DrawString(290, 120, "** WIN **", Color(255, 220, 0).Code());
        } else {
            DrawString(300, 120, "LOSE...",   Color(150, 150, 150).Code());
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ② Race time
    if (s_itemAlpha[1] > 0.0f) {
        int mn, sc, ms;
        FormatTime(g_gameData.lastTime, &mn, &sc, &ms);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[1]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 0,
                         Color::WHITE.Code(),
                         "Time        :  %02d:%02d.%02d", mn, sc, ms);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ③ Best time
    if (s_itemAlpha[2] > 0.0f) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[2]);
        if (g_gameData.bestTime < 0.0f) {
            DrawString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 1,
                       "Best Time   :  --:--.--", Color::WHITE.Code());
        } else {
            int mn, sc, ms;
            FormatTime(g_gameData.bestTime, &mn, &sc, &ms);
            DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 1,
                             Color::WHITE.Code(),
                             "Best Time   :  %02d:%02d.%02d%s",
                             mn, sc, ms,
                             s_isBestUpdated ? "  [NEW RECORD!]" : "");
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ④ Prize
    if (s_itemAlpha[3] > 0.0f) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[3]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 2,
                         Color(255, 220, 0).Code(),
                         "Prize       :  + $%d", s_prize);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ⑤ Wallet
    if (s_itemAlpha[4] > 0.0f) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[4]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 3,
                         Color::WHITE.Code(),
                         "Wallet      :  $%d", g_gameData.money);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // PRESS ANY KEY (blink)
    if (s_allShown) {
        if ((s_blinkTimer / BLINK_INTERVAL) % 2 == 0) {
            DrawString(275, 520, "PRESS ANY KEY", Color::WHITE.Code());
        }
    }
}
