#include "../DxLib/DxLib.h"
#include "share.h"
#include "screen_manager.h"
#include "input/garage_input.h"
#include "game_data.h"
#include "garage.h"
#include "dealer.h"
#include "customize.h"
#include "game_screen.h"
#include <string.h>

// ── Constants ────────────────────────────────────────────────
#define MENU_X       300
#define MENU_Y       430
#define MENU_LINE_H   50
#define CAR_SPRITE_W 200
#define CAR_SPRITE_H 120
#define CAR_SPRITE_X ((800 - CAR_SPRITE_W) / 2)
#define CAR_SPRITE_Y 260

// ── Menu definition ──────────────────────────────────────────
typedef enum {
    GARAGE_MENU_DEALER = 0,
    GARAGE_MENU_CUSTOM = 1,
    GARAGE_MENU_RACE   = 2,
    GARAGE_MENU_COUNT  = 3,
} GARAGE_MENU;

static const char* MENU_LABELS[GARAGE_MENU_COUNT] = {
    "Car Shop",
    "Customize",
    "Race Start",
};

// Background colors used as placeholders until art is ready
static const int BG_COLORS[GARAGE_MENU_COUNT][3] = {
    {  40,  30,  20 },   // Dealer  : dark brown
    {  20,  30,  50 },   // Garage  : dark blue-gray
    {  15,  40,  15 },   // Street  : dark green
};

// ── State ────────────────────────────────────────────────────
static int s_menuIndex = GARAGE_MENU_CUSTOM;
static int s_carIndex  = 0;
static GarageInput s_input;

// Loaded image handles (-1 = not loaded / placeholder)
static int s_hBgDealer  = -1;
static int s_hBgGarage  = -1;
static int s_hBgStreet  = -1;

// ── Resource loading ─────────────────────────────────────────
static void LoadGarageResources(void) {
    s_hBgDealer = LoadGraph("resource/garage/bg_dealer.png");
    s_hBgGarage = LoadGraph("resource/garage/bg_garage.png");
    s_hBgStreet = LoadGraph("resource/garage/bg_street.png");

    // Load car sprites for each owned car
    for (int i = 0; i < g_gameData.carCount; i++) {
        char path[128];
        sprintf_s(path, sizeof(path), "resource/cars/car_%02d.png", i);
        g_gameData.cars[i].hSprite = LoadGraph(path);
    }
}

// ── Helpers ──────────────────────────────────────────────────

// Draw background: use loaded image if available, else colored rect + label
static void DrawBg(int hImg, int colorIndex) {
    if (hImg >= 0) {
        DrawExtendGraph(0, 0, 800, 600, hImg, TRUE);
    } else {
        int r = BG_COLORS[colorIndex][0];
        int g = BG_COLORS[colorIndex][1];
        int b = BG_COLORS[colorIndex][2];
        DrawFillBox(0, 0, 800, 600, Color(r, g, b).Code());
    }
}

// Draw car sprite: use loaded image if available, else a simple rectangle
static void DrawCarSprite(int hSprite, int x, int y, int w, int h, const char* name) {
    if (hSprite >= 0) {
        DrawGraph(x, y, hSprite, TRUE);
    } else {
        // Placeholder: dark rectangle with car name
        DrawBox(x, y, x + w, y + h, Color(80, 80, 80).Code(), TRUE);
        DrawBox(x, y, x + w, y + h, Color(160, 160, 160).Code(), FALSE);
        // Simple car silhouette
        DrawBox(x + 20, y + 30,  x + w - 20, y + h - 20, Color(100, 120, 160).Code(), TRUE);
        DrawBox(x + 50, y + 10,  x + w - 50, y + 40,     Color(140, 160, 200).Code(), TRUE);
        DrawFillBox(x + 15,      y + h - 30, x + 45,      y + h - 10, Color(40, 40, 40).Code());
        DrawFillBox(x + w - 45,  y + h - 30, x + w - 15,  y + h - 10, Color(40, 40, 40).Code());
        DrawString(x + 10, y + h / 2 - 8, name, Color(220, 220, 220).Code());
    }
}

// ── Screen functions ──────────────────────────────────────────
void UpdateGarageInput(void) {
    s_input.Update();
}

void UpdateGarage(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        s_menuIndex   = GARAGE_MENU_CUSTOM;
        s_carIndex    = 0;
        LoadGarageResources();
        s_input.Update();   // re-sync: suppress false triggers from keys held across screens
        s_initialized = 1;
    }

    // Vertical: menu selection
    if (s_input.IsMenuUpTriggered()) {
        s_menuIndex = (s_menuIndex - 1 + GARAGE_MENU_COUNT) % GARAGE_MENU_COUNT;
    }
    if (s_input.IsMenuDownTriggered()) {
        s_menuIndex = (s_menuIndex + 1) % GARAGE_MENU_COUNT;
    }

    // Horizontal: car selection (only when 2+ cars owned)
    if (g_gameData.carCount > 1) {
        if (s_input.IsCarPrevTriggered()) {
            s_carIndex = (s_carIndex - 1 + g_gameData.carCount) % g_gameData.carCount;
        }
        if (s_input.IsCarNextTriggered()) {
            s_carIndex = (s_carIndex + 1) % g_gameData.carCount;
        }
    }

    // Confirm
    if (s_input.IsDecideTriggered()) {
        g_gameData.selectedCarIndex = s_carIndex;
        s_initialized = 0;  // reset on next entry
        switch (s_menuIndex) {
        case GARAGE_MENU_DEALER: ChangeScreen(UpdateDealerInput,    UpdateDealer,    DrawDealer);    break;
        case GARAGE_MENU_CUSTOM: ChangeScreen(UpdateCustomizeInput, UpdateCustomize, DrawCustomize); break;
        case GARAGE_MENU_RACE:   ChangeScreen(NULL,                 UpdateGame,      DrawGame);      break;
        }
    }
}

void DrawGarage(void) {
    PlayerCar* car = &g_gameData.cars[s_carIndex];

    // Background (switches per menu)
    int hBgTable[GARAGE_MENU_COUNT] = { s_hBgDealer, s_hBgGarage, s_hBgStreet };
    DrawBg(hBgTable[s_menuIndex], s_menuIndex);

    // Horizontal arrow hints (multiple cars only)
    if (g_gameData.carCount > 1) {
        DrawString(CAR_SPRITE_X - 28, CAR_SPRITE_Y + CAR_SPRITE_H / 2 - 8,
                   "<", Color::WHITE.Code());
        DrawString(CAR_SPRITE_X + CAR_SPRITE_W + 12, CAR_SPRITE_Y + CAR_SPRITE_H / 2 - 8,
                   ">", Color::WHITE.Code());
    }

    // Car sprite
    DrawCarSprite(car->hSprite, CAR_SPRITE_X, CAR_SPRITE_Y,
                  CAR_SPRITE_W, CAR_SPRITE_H, car->name);

    // Car stats (below sprite)
    DrawFormatString(CAR_SPRITE_X, CAR_SPRITE_Y + CAR_SPRITE_H + 8,
                     Color(180, 180, 180).Code(),
                     "%.0f km/h  Gear:%d",
                     car->maxSpeed, car->gearCount);

    // Money display (top right)
    DrawFormatString(600, 10, Color(255, 220, 0).Code(), "$ %d", g_gameData.money);

    // Vertical menu
    for (int i = 0; i < GARAGE_MENU_COUNT; i++) {
        int isSel = (i == s_menuIndex);
        int col   = isSel ? Color::YELLOW.Code() : Color(180, 180, 180).Code();
        DrawFormatString(MENU_X, MENU_Y + i * MENU_LINE_H, col,
                         "%s %s", isSel ? ">" : " ", MENU_LABELS[i]);
    }

    // Controls hint
    DrawString(10, 575, "[Z/Enter] Select  [Up/Down] Menu  [Left/Right] Car",
               Color(100, 100, 100).Code());
}
