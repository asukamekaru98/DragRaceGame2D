#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/game_data.h"
#include "../header/car_data.h"
#include "../header/dealer.h"
#include "../header/garage.h"
#include <string.h>
#include <stdio.h>

// ── Constants ────────────────────────────────────────────────
#define CARD_WIDTH    160
#define CARD_HEIGHT   130
#define CARD_MARGIN    20
#define CARD_STEP     (CARD_WIDTH + CARD_MARGIN)
#define CARD_Y        100
#define SCROLL_LERP   0.18f

#define DETAIL_X      180
#define DETAIL_Y      310
#define DETAIL_LINE    28

#define BTN_Y         530
#define BTN_BUY_X     260
#define BTN_BACK_X    420

// ── State ────────────────────────────────────────────────────
typedef enum {
    DEALER_STATE_CARD,   // browsing cards with left/right
    DEALER_STATE_MENU,   // buy / back button focused
} DEALER_STATE;

static DEALER_STATE s_state      = DEALER_STATE_CARD;
static SI_4         s_carIndex   = 0;
static F_4          s_scrollX    = 0.0f;
static F_4          s_targetX    = 0.0f;
static SI_4         s_cursorPos  = 0;       // 0=Buy  1=Back
static SI_4         s_cantAfford = 0;       // flash timer for "cannot afford"
static SI_4         s_hBg        = -1;
static SI_4         s_hSprites[16];         // sprite handles per CAR_TABLE entry

// ── Helpers ──────────────────────────────────────────────────
static SI_4 CanAfford(SI_4 idx) {
    return g_gameData.money >= CAR_TABLE[idx].price;
}

static SI_4 AlreadyOwned(SI_4 idx) {
    for (int i = 0; i < g_gameData.carCount; i++) {
        if (strcmp(g_gameData.cars[i].name, CAR_TABLE[idx].name) == 0) return 1;
    }
    return 0;
}

static void BuyCar(SI_4 idx) {
    if (g_gameData.carCount >= MAX_GARAGE_CARS) return;

    g_gameData.money -= CAR_TABLE[idx].price;

    PlayerCar* car = &g_gameData.cars[g_gameData.carCount];
    strcpy_s(car->name, sizeof(car->name), CAR_TABLE[idx].name);
    car->hSprite      = s_hSprites[idx];
    car->maxSpeed     = CAR_TABLE[idx].maxSpeed;
    car->acceleration = CAR_TABLE[idx].acceleration;
    car->gearCount    = CAR_TABLE[idx].gearCount;
    car->price        = CAR_TABLE[idx].price;
    for (int i = 0; i < PARTS_CAT_COUNT; i++) car->equippedParts[i] = -1;

    g_gameData.carCount++;
}

// Draw a car card (placeholder if no sprite loaded)
static void DrawCard(SI_4 cardX, SI_4 cardY, SI_4 idx, SI_4 selected) {
    const CarData* car = &CAR_TABLE[idx];

    // Card body
    SI_4 bgCol = selected ? GetColor(50, 50, 70) : GetColor(30, 30, 40);
    DrawFillBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT, bgCol);

    // Sprite or placeholder silhouette
    if (s_hSprites[idx] >= 0) {
        DrawExtendGraph(cardX + 5, cardY + 5,
                        cardX + CARD_WIDTH - 5, cardY + CARD_HEIGHT - 30,
                        s_hSprites[idx], TRUE);
    } else {
        // Simple silhouette
        SI_4 sx = cardX + 15, sy = cardY + 20;
        SI_4 sw = CARD_WIDTH - 30, sh = CARD_HEIGHT - 50;
        DrawFillBox(sx,          sy + sh / 3, sx + sw,      sy + sh,     GetColor(80, 100, 140));
        DrawFillBox(sx + sw / 4, sy,          sx + sw * 3/4, sy + sh / 2, GetColor(110, 130, 170));
        DrawFillBox(sx + 5,      sy + sh - 8, sx + 25,      sy + sh + 5, GetColor(30, 30, 30));
        DrawFillBox(sx + sw - 25,sy + sh - 8, sx + sw - 5,  sy + sh + 5, GetColor(30, 30, 30));
    }

    // Cannot afford: dark overlay
    if (!CanAfford(idx)) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawFillBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT, GetColor(0, 0, 0));
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // Already owned: green tint overlay
    if (AlreadyOwned(idx)) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
        DrawFillBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT, GetColor(0, 200, 0));
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // Border (bright yellow for selected)
    SI_4 borderCol = selected ? GetColor(255, 255, 0) : GetColor(80, 80, 100);
    DrawBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT, borderCol, FALSE);

    // Car name
    DrawString(cardX + 5, cardY + CARD_HEIGHT - 22, car->name, GetColor(220, 220, 220));

    // Price below card
    SI_4 priceCol = CanAfford(idx) ? GetColor(255, 220, 80) : GetColor(160, 80, 80);
    DrawFormatString(cardX + 5, cardY + CARD_HEIGHT + 6, priceCol, "$%d", car->price);
}

// ── Resource loading ─────────────────────────────────────────
static void LoadDealerResources(void) {
    s_hBg = LoadGraph("resource/garage/bg_dealer.png");

    for (int i = 0; i < CAR_TABLE_COUNT; i++) {
        char path[128];
        sprintf_s(path, sizeof(path), "resource/cars/dealer_car_%02d.png", i);
        s_hSprites[i] = LoadGraph(path);
    }
}

static void UnloadDealerResources(void) {
    if (s_hBg >= 0) { DeleteGraph(s_hBg); s_hBg = -1; }
    for (int i = 0; i < CAR_TABLE_COUNT; i++) {
        if (s_hSprites[i] >= 0) { DeleteGraph(s_hSprites[i]); s_hSprites[i] = -1; }
    }
}

// ── Screen functions ──────────────────────────────────────────
void UpdateDealer(void) {
    static SI_4 s_initialized = 0;
    if (!s_initialized) {
        s_state     = DEALER_STATE_CARD;
        s_carIndex  = 0;
        s_scrollX   = 0.0f;
        s_targetX   = 0.0f;
        s_cursorPos = 0;
        s_cantAfford = 0;
        LoadDealerResources();
        s_initialized = 1;
    }

    // Scroll animation every frame
    s_scrollX += (s_targetX - s_scrollX) * SCROLL_LERP;

    // Flash timer
    if (s_cantAfford > 0) s_cantAfford--;

    switch (s_state) {

    case DEALER_STATE_CARD:
        if (IsKeyTriggered(KEY_INPUT_LEFT)) {
            s_carIndex = (s_carIndex - 1 + CAR_TABLE_COUNT) % CAR_TABLE_COUNT;
            s_targetX  = -(F_4)(s_carIndex * CARD_STEP);
        }
        if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
            s_carIndex = (s_carIndex + 1) % CAR_TABLE_COUNT;
            s_targetX  = -(F_4)(s_carIndex * CARD_STEP);
        }
        if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
            s_state     = DEALER_STATE_MENU;
            s_cursorPos = 0;
        }
        if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            s_initialized = 0;
            UnloadDealerResources();
            ChangeScreen(UpdateGarage, DrawGarage);
        }
        break;

    case DEALER_STATE_MENU:
        if (IsKeyTriggered(KEY_INPUT_LEFT) || IsKeyTriggered(KEY_INPUT_RIGHT)) {
            s_cursorPos = 1 - s_cursorPos;
        }
        if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
            if (s_cursorPos == 0) {
                // Buy
                if (AlreadyOwned(s_carIndex)) {
                    // already have it — treat like back
                    s_state = DEALER_STATE_CARD;
                } else if (CanAfford(s_carIndex)) {
                    BuyCar(s_carIndex);
                    s_initialized = 0;
                    UnloadDealerResources();
                    ChangeScreen(UpdateGarage, DrawGarage);
                } else {
                    s_cantAfford = 90;   // flash for ~1.5s
                }
            } else {
                // Back
                s_state = DEALER_STATE_CARD;
            }
        }
        if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            s_state = DEALER_STATE_CARD;
        }
        break;
    }
}

void DrawDealer(void) {
    const CarData* car = &CAR_TABLE[s_carIndex];

    // Background
    if (s_hBg >= 0) {
        DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);
    } else {
        DrawFillBox(0, 0, 800, 600, GetColor(25, 20, 35));
        DrawString(10, 10, "CAR DEALER", GetColor(200, 160, 80));
    }

    // ── Card row ─────────────────────────────────────────────
    // Cards are centered: selected card center = screen center (400)
    SI_4 centerX = 400 - CARD_WIDTH / 2;
    for (int i = 0; i < CAR_TABLE_COUNT; i++) {
        SI_4 cardX = centerX + (SI_4)s_scrollX + i * CARD_STEP;
        // Cull cards that are fully off-screen
        if (cardX + CARD_WIDTH < 0 || cardX > 800) continue;
        DrawCard(cardX, CARD_Y, i, (i == s_carIndex));
    }

    // Arrow hints
    if (s_carIndex > 0) {
        DrawString(20, CARD_Y + CARD_HEIGHT / 2, "<", GetColor(200, 200, 200));
    }
    if (s_carIndex < CAR_TABLE_COUNT - 1) {
        DrawString(770, CARD_Y + CARD_HEIGHT / 2, ">", GetColor(200, 200, 200));
    }

    // ── Detail panel ─────────────────────────────────────────
    // Acceleration stars (0.0-5.0 → 0-5 stars)
    char stars[8] = { 0 };
    SI_4 fullStars = (SI_4)(car->acceleration + 0.5f);
    for (int i = 0; i < 5; i++) stars[i] = (i < fullStars) ? '*' : '-';

    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 0,
                     GetColor(255, 255, 255), "Name      : %s", car->name);
    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 1,
                     GetColor(255, 255, 255), "Top Speed : %.0f km/h", car->maxSpeed);
    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 2,
                     GetColor(255, 255, 255), "Accel     : [%s]", stars);
    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 3,
                     GetColor(255, 255, 255), "Gears     : %d", car->gearCount);
    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 4,
                     GetColor(255, 220, 80),  "Price     : $%d", car->price);

    SI_4 moneyCol = CanAfford(s_carIndex) ? GetColor(255, 255, 255) : GetColor(255, 80, 80);
    DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 5, moneyCol,
                     "Wallet    : $%d%s",
                     g_gameData.money,
                     CanAfford(s_carIndex) ? "" : "  [NOT ENOUGH]");

    // Already owned label
    if (AlreadyOwned(s_carIndex)) {
        DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 6,
                   "[Already Owned]", GetColor(100, 220, 100));
    }

    // ── Buttons ──────────────────────────────────────────────
    if (s_state == DEALER_STATE_MENU) {
        SI_4 buyCol  = (s_cursorPos == 0) ? GetColor(255, 255, 0) : GetColor(180, 180, 180);
        SI_4 backCol = (s_cursorPos == 1) ? GetColor(255, 255, 0) : GetColor(180, 180, 180);

        // Highlight box on selected button
        if (s_cursorPos == 0) DrawBox(BTN_BUY_X  - 5, BTN_Y - 4, BTN_BUY_X  + 85, BTN_Y + 20, GetColor(255,255,0), FALSE);
        else                   DrawBox(BTN_BACK_X - 5, BTN_Y - 4, BTN_BACK_X + 55, BTN_Y + 20, GetColor(255,255,0), FALSE);

        DrawString(BTN_BUY_X,  BTN_Y, "[ Buy ]", buyCol);
        DrawString(BTN_BACK_X, BTN_Y, "[ Back ]", backCol);

        // Cannot-afford flash message
        if (s_cantAfford > 0 && (s_cantAfford / 8) % 2 == 0) {
            DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 7,
                       "! Not enough money !", GetColor(255, 80, 80));
        }
    } else {
        DrawString(BTN_BUY_X,  BTN_Y, "[ Buy ]",  GetColor(120, 120, 120));
        DrawString(BTN_BACK_X, BTN_Y, "[ Back ]", GetColor(120, 120, 120));
    }

    // Controls hint
    if (s_state == DEALER_STATE_CARD) {
        DrawString(10, 575, "[Left/Right] Browse  [Z/Enter] Select  [ESC] Back",
                   GetColor(90, 90, 90));
    } else {
        DrawString(10, 575, "[Left/Right] Choose  [Z/Enter] Confirm  [ESC] Cancel",
                   GetColor(90, 90, 90));
    }
}
