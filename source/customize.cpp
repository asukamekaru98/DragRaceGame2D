#include "../DxLib/DxLib.h"
#include "share.h"
#include "screen_manager.h"
#include "input.h"
#include "game_data.h"
#include "parts_data.h"
#include "customize.h"
#include "garage.h"
#include <string.h>

// ── Layout constants ─────────────────────────────────────────
#define CAR_X         30
#define CAR_Y         60
#define CAR_W        340
#define CAR_H        200

#define BAR_X         30
#define BAR_Y        290
#define BAR_MAX_W    200
#define BAR_H         14

#define TAB_X        410
#define TAB_Y         40
#define TAB_W         90

#define LIST_X       415
#define LIST_Y        90
#define LIST_LINE_H   32
#define LIST_MAX_SHOW  7    // max visible items before clipping

#define DETAIL_X     415
#define DETAIL_Y     380
#define DETAIL_LINE   26

#define BTN_EQUIP_X  430
#define BTN_BACK_X   580
#define BTN_Y        545

// ── State machine ────────────────────────────────────────────
typedef enum {
    CUST_STATE_TAB,    // category tab focused
    CUST_STATE_LIST,   // parts list focused
    CUST_STATE_MENU,   // equip / back button focused
} CUST_STATE;

static CUST_STATE s_state         = CUST_STATE_TAB;
static int        s_categoryIndex = 0;
static int        s_partsIndex    = 0;   // index into s_filteredList
static int        s_filteredList[32];    // PARTS_TABLE indices; -1 = "(none)"
static int        s_filteredCount = 0;
static int        s_cursorPos     = 0;   // 0=Equip  1=Back
static int        s_cantAfford    = 0;
static int        s_hBg           = -1;

static const char* CAT_LABELS[PARTS_CAT_COUNT] = {
    "Engine", "Tire", "Body", "Exterior"
};

// ── Stats helpers ─────────────────────────────────────────────
typedef struct { float maxSpeed; float accel; } CarStats;

static CarStats CalcStats(const PlayerCar* car) {
    CarStats s = { car->maxSpeed, car->acceleration };
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        int pi = car->equippedParts[i];
        if (pi >= 0) {
            s.maxSpeed += PARTS_TABLE[pi].fMaxSpeedBonus;
            s.accel    += PARTS_TABLE[pi].fAccelBonus;
        }
    }
    return s;
}

static CarStats CalcPreviewStats(const PlayerCar* car, int previewPI) {
    PlayerCar tmp = *car;
    if (previewPI >= 0) {
        int cat = (int)PARTS_TABLE[previewPI].eCategory;
        tmp.equippedParts[cat] = previewPI;
    } else {
        // "(none)" selected — unequip current category
        tmp.equippedParts[s_categoryIndex] = -1;
    }
    return CalcStats(&tmp);
}

// ── Filtered list ─────────────────────────────────────────────
static void RebuildFilteredList(void) {
    s_filteredCount = 0;
    // First entry is always "(none)" to allow unequipping
    s_filteredList[s_filteredCount++] = -1;
    for (int i = 0; i < PARTS_TABLE_COUNT; i++) {
        if ((int)PARTS_TABLE[i].eCategory == s_categoryIndex) {
            s_filteredList[s_filteredCount++] = i;
        }
    }
    s_partsIndex = 0;
}

// ── Equip ─────────────────────────────────────────────────────
static void EquipParts(int pi) {
    PlayerCar* car = &g_gameData.cars[g_gameData.selectedCarIndex];
    if (pi >= 0) {
        g_gameData.money -= PARTS_TABLE[pi].iPrice;
        car->equippedParts[s_categoryIndex] = pi;
    } else {
        // Unequip — no cost, no refund
        car->equippedParts[s_categoryIndex] = -1;
    }
}

// ── Draw helpers ──────────────────────────────────────────────
static void DrawStatBar(int x, int y, const char* label,
                        float cur, float pre, float maxVal) {
    int curW = (int)(cur / maxVal * BAR_MAX_W);
    int preW = (int)(pre / maxVal * BAR_MAX_W);
    if (curW > BAR_MAX_W) curW = BAR_MAX_W;
    if (preW > BAR_MAX_W) preW = BAR_MAX_W;

    DrawString(x, y, label, Color(200, 200, 200).Code());

    int bx = x + 90;

    // Background track
    DrawFillBox(bx, y + 1, bx + BAR_MAX_W, y + BAR_H - 1, Color(50, 50, 50).Code());

    // Current value (white)
    if (curW > 0)
        DrawFillBox(bx, y + 1, bx + curW, y + BAR_H - 1, Color(180, 180, 180).Code());

    // Preview delta
    if (preW > curW) {
        // increase: yellow
        DrawFillBox(bx + curW, y + 1, bx + preW, y + BAR_H - 1, Color(255, 220, 0).Code());
    } else if (preW < curW) {
        // decrease: red mark over lost portion
        DrawFillBox(bx + preW, y + 1, bx + curW, y + BAR_H - 1, Color(200, 60, 60).Code());
    }

    // Bar outline
    DrawBox(bx, y, bx + BAR_MAX_W, y + BAR_H, Color(100, 100, 100).Code(), FALSE);

    // Values
    if (pre != cur) {
        int deltaCol = (pre > cur) ? Color(255, 220, 0).Code() : Color(255, 80, 80).Code();
        DrawFormatString(bx + BAR_MAX_W + 6, y, Color(200, 200, 200).Code(),
                         "%.0f", cur);
        DrawFormatString(bx + BAR_MAX_W + 46, y, deltaCol,
                         "-> %.0f", pre);
    } else {
        DrawFormatString(bx + BAR_MAX_W + 6, y, Color(200, 200, 200).Code(),
                         "%.0f", cur);
    }
}

static void DrawCarPlaceholder(int x, int y, int w, int h, const char* name) {
    DrawFillBox(x, y, x + w, y + h, Color(30, 35, 50).Code());
    DrawBox(x, y, x + w, y + h, Color(70, 80, 110).Code(), FALSE);
    // body
    DrawFillBox(x + 20,      y + h/3,     x + w - 20, y + h - 25, Color(70, 90, 140).Code());
    // roof
    DrawFillBox(x + w/4,     y + 15,      x + w*3/4,  y + h/2,    Color(90, 110, 160).Code());
    // wheels
    DrawFillBox(x + 20,      y + h - 30,  x + 55,     y + h - 10, Color(30, 30, 30).Code());
    DrawFillBox(x + w - 55,  y + h - 30,  x + w - 20, y + h - 10, Color(30, 30, 30).Code());
    DrawString(x + 8, y + h - 55, name, Color(200, 200, 200).Code());
}

// ── Resource ──────────────────────────────────────────────────
static void LoadCustomizeResources(void) {
    s_hBg = LoadGraph("resource/garage/bg_garage.png");
}

static void UnloadCustomizeResources(void) {
    if (s_hBg >= 0) { DeleteGraph(s_hBg); s_hBg = -1; }
}

// ── Screen functions ──────────────────────────────────────────
void UpdateCustomize(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        s_state         = CUST_STATE_TAB;
        s_categoryIndex = 0;
        s_cursorPos     = 0;
        s_cantAfford    = 0;
        RebuildFilteredList();
        LoadCustomizeResources();
        s_initialized   = 1;
    }

    if (s_cantAfford > 0) s_cantAfford--;

    switch (s_state) {

    // ── Category tab ────────────────────────────────────────
    case CUST_STATE_TAB:
        if (IsKeyTriggered(KEY_INPUT_LEFT)) {
            s_categoryIndex = (s_categoryIndex - 1 + PARTS_CAT_COUNT) % PARTS_CAT_COUNT;
            RebuildFilteredList();
        }
        if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
            s_categoryIndex = (s_categoryIndex + 1) % PARTS_CAT_COUNT;
            RebuildFilteredList();
        }
        if (IsKeyTriggered(KEY_INPUT_DOWN)) {
            s_state = CUST_STATE_LIST;
        }
        if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            s_initialized = 0;
            UnloadCustomizeResources();
            ChangeScreen(UpdateGarage, DrawGarage);
        }
        break;

    // ── Parts list ──────────────────────────────────────────
    case CUST_STATE_LIST:
        if (IsKeyTriggered(KEY_INPUT_UP)) {
            if (s_partsIndex == 0) {
                s_state = CUST_STATE_TAB;
            } else {
                s_partsIndex--;
            }
        }
        if (IsKeyTriggered(KEY_INPUT_DOWN)) {
            if (s_partsIndex < s_filteredCount - 1) s_partsIndex++;
        }
        if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
            s_state     = CUST_STATE_MENU;
            s_cursorPos = 0;
        }
        if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            s_initialized = 0;
            UnloadCustomizeResources();
            ChangeScreen(UpdateGarage, DrawGarage);
        }
        break;

    // ── Equip / Back ─────────────────────────────────────────
    case CUST_STATE_MENU:
        if (IsKeyTriggered(KEY_INPUT_LEFT) || IsKeyTriggered(KEY_INPUT_RIGHT)) {
            s_cursorPos = 1 - s_cursorPos;
        }
        if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
            if (s_cursorPos == 0) {
                int pi = s_filteredList[s_partsIndex];
                // free if unequipping or affordable
                int cost = (pi >= 0) ? PARTS_TABLE[pi].iPrice : 0;
                if (g_gameData.money >= cost) {
                    EquipParts(pi);
                    s_initialized = 0;
                    UnloadCustomizeResources();
                    ChangeScreen(UpdateGarage, DrawGarage);
                } else {
                    s_cantAfford = 90;
                }
            } else {
                s_state = CUST_STATE_LIST;
            }
        }
        if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            s_state = CUST_STATE_LIST;
        }
        break;
    }
}

void DrawCustomize(void) {
    PlayerCar* car = &g_gameData.cars[g_gameData.selectedCarIndex];
    int pi = s_filteredList[s_partsIndex];   // -1 = "(none)"

    // ── Background ───────────────────────────────────────────
    if (s_hBg >= 0) {
        DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);
    } else {
        DrawFillBox(0, 0, 800, 600, Color(20, 25, 40).Code());
    }

    // ── Left: Car sprite ─────────────────────────────────────
    if (car->hSprite >= 0) {
        DrawExtendGraph(CAR_X, CAR_Y, CAR_X + CAR_W, CAR_Y + CAR_H, car->hSprite, TRUE);
    } else {
        DrawCarPlaceholder(CAR_X, CAR_Y, CAR_W, CAR_H, car->name);
    }

    // Wallet top-left
    DrawFormatString(CAR_X, CAR_Y - 22, Color(255, 220, 0).Code(),
                     "Wallet: $%d", g_gameData.money);

    // ── Left: Stat bars ──────────────────────────────────────
    CarStats cur  = CalcStats(car);
    CarStats prev = CalcPreviewStats(car, pi);

    DrawString(BAR_X, BAR_Y - 20, "Performance", Color(160, 160, 160).Code());
    DrawStatBar(BAR_X, BAR_Y,      "Top Speed", cur.maxSpeed, prev.maxSpeed, 400.0f);
    DrawStatBar(BAR_X, BAR_Y + 35, "Accel    ", cur.accel,    prev.accel,      6.0f);

    // ── Right: Category tabs ─────────────────────────────────
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        int isSel = (i == s_categoryIndex);
        int isTab = (s_state == CUST_STATE_TAB);
        int col;
        if (isSel && isTab) col = Color::YELLOW.Code();
        else if (isSel)     col = Color(200, 200, 100).Code();
        else                col = Color(140, 140, 140).Code();

        DrawFormatString(TAB_X + i * TAB_W, TAB_Y, col,
                         "%s%s", isSel ? "[" : " ", CAT_LABELS[i]);
        if (isSel) DrawFormatString(TAB_X + i * TAB_W + (int)(strlen(CAT_LABELS[i]) * 8), TAB_Y, col, "]");
    }
    DrawLine(TAB_X - 5, TAB_Y + 18, 790, TAB_Y + 18, Color(80, 80, 80).Code());

    // ── Right: Parts list ────────────────────────────────────
    int equippedPI = car->equippedParts[s_categoryIndex];

    for (int i = 0; i < s_filteredCount && i < LIST_MAX_SHOW; i++) {
        int listPI  = s_filteredList[i];
        int isSel   = (i == s_partsIndex);
        int isEquip = (listPI == equippedPI) || (listPI == -1 && equippedPI == -1);

        int col;
        if (isSel && s_state == CUST_STATE_LIST) col = Color::YELLOW.Code();
        else if (isSel)                           col = Color(200, 200, 100).Code();
        else                                      col = Color(160, 160, 160).Code();

        if (listPI == -1) {
            DrawFormatString(LIST_X, LIST_Y + i * LIST_LINE_H, col,
                             "%s (none)%s",
                             isSel ? ">" : " ",
                             isEquip ? " [equipped]" : "");
        } else {
            int affordable = (g_gameData.money >= PARTS_TABLE[listPI].iPrice);
            int nameCol = affordable ? col : Color(120, 80, 80).Code();
            DrawFormatString(LIST_X, LIST_Y + i * LIST_LINE_H, nameCol,
                             "%s %-18s $%-5d%s",
                             isSel ? ">" : " ",
                             PARTS_TABLE[listPI].pName,
                             PARTS_TABLE[listPI].iPrice,
                             isEquip ? " [equipped]" : "");
        }
    }

    // ── Right: Part detail panel ─────────────────────────────
    DrawLine(TAB_X - 5, DETAIL_Y - 10, 790, DETAIL_Y - 10, Color(80, 80, 80).Code());

    if (pi == -1) {
        DrawString(DETAIL_X, DETAIL_Y, "(none) - unequip current part", Color(160, 160, 160).Code());
        DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE, "No cost / no refund", Color(120, 120, 120).Code());
    } else {
        DrawFormatString(DETAIL_X, DETAIL_Y,
                         Color::WHITE.Code(), "%s", PARTS_TABLE[pi].pName);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 1,
                         Color(200, 200, 200).Code(),
                         "Top Speed  %+.0f km/h", PARTS_TABLE[pi].fMaxSpeedBonus);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 2,
                         Color(200, 200, 200).Code(),
                         "Accel      %+.2f", PARTS_TABLE[pi].fAccelBonus);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 3,
                         Color(255, 220, 80).Code(),
                         "Price      $%d", PARTS_TABLE[pi].iPrice);

        int moneyCol = (g_gameData.money >= PARTS_TABLE[pi].iPrice)
                        ? Color::WHITE.Code() : Color(255, 80, 80).Code();
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 4, moneyCol,
                         "Wallet     $%d%s",
                         g_gameData.money,
                         (g_gameData.money >= PARTS_TABLE[pi].iPrice) ? "" : "  [NOT ENOUGH]");
    }

    // Cannot-afford flash
    if (s_cantAfford > 0 && (s_cantAfford / 8) % 2 == 0) {
        DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 5,
                   "! Not enough money !", Color(255, 80, 80).Code());
    }

    // ── Buttons ──────────────────────────────────────────────
    int showMenu = (s_state == CUST_STATE_MENU);
    int equipCol = (showMenu && s_cursorPos == 0) ? Color::YELLOW.Code() : Color(130, 130, 130).Code();
    int backCol  = (showMenu && s_cursorPos == 1) ? Color::YELLOW.Code() : Color(130, 130, 130).Code();

    if (showMenu) {
        if (s_cursorPos == 0)
            DrawBox(BTN_EQUIP_X - 4, BTN_Y - 3, BTN_EQUIP_X + 80, BTN_Y + 19,
                    Color::YELLOW.Code(), FALSE);
        else
            DrawBox(BTN_BACK_X - 4, BTN_Y - 3, BTN_BACK_X + 65, BTN_Y + 19,
                    Color::YELLOW.Code(), FALSE);
    }
    DrawString(BTN_EQUIP_X, BTN_Y, "[ Equip ]", equipCol);
    DrawString(BTN_BACK_X,  BTN_Y, "[ Back ]",  backCol);

    // ── Controls hint ────────────────────────────────────────
    const char* hint = "";
    switch (s_state) {
    case CUST_STATE_TAB:  hint = "[Left/Right] Category  [Down] Parts list  [ESC] Back"; break;
    case CUST_STATE_LIST: hint = "[Up/Down] Select  [Z/Enter] Confirm  [ESC] Back";      break;
    case CUST_STATE_MENU: hint = "[Left/Right] Choose  [Z/Enter] Decide  [ESC] Cancel";  break;
    }
    DrawString(10, 578, hint, Color(80, 80, 80).Code());
}
