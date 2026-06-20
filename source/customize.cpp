#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/game_data.h"
#include "../header/parts_data.h"
#include "../header/customize.h"
#include "../header/garage.h"
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
static SI_4       s_categoryIndex = 0;
static SI_4       s_partsIndex    = 0;   // index into s_filteredList
static SI_4       s_filteredList[32];    // PARTS_TABLE indices; -1 = "(none)"
static SI_4       s_filteredCount = 0;
static SI_4       s_cursorPos     = 0;   // 0=Equip  1=Back
static SI_4       s_cantAfford    = 0;
static SI_4       s_hBg           = -1;

static const char* CAT_LABELS[PARTS_CAT_COUNT] = {
    "Engine", "Tire", "Body", "Exterior"
};

// ── Stats helpers ─────────────────────────────────────────────
typedef struct { F_4 maxSpeed; F_4 accel; } CarStats;

static CarStats CalcStats(const PlayerCar* car) {
    CarStats s = { car->maxSpeed, car->acceleration };
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        SI_4 pi = car->equippedParts[i];
        if (pi >= 0) {
            s.maxSpeed += PARTS_TABLE[pi].maxSpeedBonus;
            s.accel    += PARTS_TABLE[pi].accelBonus;
        }
    }
    return s;
}

static CarStats CalcPreviewStats(const PlayerCar* car, SI_4 previewPI) {
    PlayerCar tmp = *car;
    if (previewPI >= 0) {
        SI_4 cat = (SI_4)PARTS_TABLE[previewPI].category;
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
        if ((SI_4)PARTS_TABLE[i].category == s_categoryIndex) {
            s_filteredList[s_filteredCount++] = i;
        }
    }
    s_partsIndex = 0;
}

// ── Equip ─────────────────────────────────────────────────────
static void EquipParts(SI_4 pi) {
    PlayerCar* car = &g_gameData.cars[g_gameData.selectedCarIndex];
    if (pi >= 0) {
        g_gameData.money -= PARTS_TABLE[pi].price;
        car->equippedParts[s_categoryIndex] = pi;
    } else {
        // Unequip — no cost, no refund
        car->equippedParts[s_categoryIndex] = -1;
    }
}

// ── Draw helpers ──────────────────────────────────────────────
static void DrawStatBar(SI_4 x, SI_4 y, const char* label,
                        F_4 cur, F_4 pre, F_4 maxVal) {
    SI_4 curW = (SI_4)(cur / maxVal * BAR_MAX_W);
    SI_4 preW = (SI_4)(pre / maxVal * BAR_MAX_W);
    if (curW > BAR_MAX_W) curW = BAR_MAX_W;
    if (preW > BAR_MAX_W) preW = BAR_MAX_W;

    DrawString(x, y, label, GetColor(200, 200, 200));

    SI_4 bx = x + 90;

    // Background track
    DrawFillBox(bx, y + 1, bx + BAR_MAX_W, y + BAR_H - 1, GetColor(50, 50, 50));

    // Current value (white)
    if (curW > 0)
        DrawFillBox(bx, y + 1, bx + curW, y + BAR_H - 1, GetColor(180, 180, 180));

    // Preview delta
    if (preW > curW) {
        // increase: yellow
        DrawFillBox(bx + curW, y + 1, bx + preW, y + BAR_H - 1, GetColor(255, 220, 0));
    } else if (preW < curW) {
        // decrease: red mark over lost portion
        DrawFillBox(bx + preW, y + 1, bx + curW, y + BAR_H - 1, GetColor(200, 60, 60));
    }

    // Bar outline
    DrawBox(bx, y, bx + BAR_MAX_W, y + BAR_H, GetColor(100, 100, 100), FALSE);

    // Values
    if (pre != cur) {
        SI_4 deltaCol = (pre > cur) ? GetColor(255, 220, 0) : GetColor(255, 80, 80);
        DrawFormatString(bx + BAR_MAX_W + 6, y, GetColor(200, 200, 200),
                         "%.0f", cur);
        DrawFormatString(bx + BAR_MAX_W + 46, y, deltaCol,
                         "-> %.0f", pre);
    } else {
        DrawFormatString(bx + BAR_MAX_W + 6, y, GetColor(200, 200, 200),
                         "%.0f", cur);
    }
}

static void DrawCarPlaceholder(SI_4 x, SI_4 y, SI_4 w, SI_4 h, const char* name) {
    DrawFillBox(x, y, x + w, y + h, GetColor(30, 35, 50));
    DrawBox(x, y, x + w, y + h, GetColor(70, 80, 110), FALSE);
    // body
    DrawFillBox(x + 20,      y + h/3,     x + w - 20, y + h - 25, GetColor(70, 90, 140));
    // roof
    DrawFillBox(x + w/4,     y + 15,      x + w*3/4,  y + h/2,    GetColor(90, 110, 160));
    // wheels
    DrawFillBox(x + 20,      y + h - 30,  x + 55,     y + h - 10, GetColor(30, 30, 30));
    DrawFillBox(x + w - 55,  y + h - 30,  x + w - 20, y + h - 10, GetColor(30, 30, 30));
    DrawString(x + 8, y + h - 55, name, GetColor(200, 200, 200));
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
    static SI_4 s_initialized = 0;
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
                SI_4 pi = s_filteredList[s_partsIndex];
                // free if unequipping or affordable
                SI_4 cost = (pi >= 0) ? PARTS_TABLE[pi].price : 0;
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
    SI_4 pi = s_filteredList[s_partsIndex];   // -1 = "(none)"

    // ── Background ───────────────────────────────────────────
    if (s_hBg >= 0) {
        DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);
    } else {
        DrawFillBox(0, 0, 800, 600, GetColor(20, 25, 40));
    }

    // ── Left: Car sprite ─────────────────────────────────────
    if (car->hSprite >= 0) {
        DrawExtendGraph(CAR_X, CAR_Y, CAR_X + CAR_W, CAR_Y + CAR_H, car->hSprite, TRUE);
    } else {
        DrawCarPlaceholder(CAR_X, CAR_Y, CAR_W, CAR_H, car->name);
    }

    // Wallet top-left
    DrawFormatString(CAR_X, CAR_Y - 22, GetColor(255, 220, 0),
                     "Wallet: $%d", g_gameData.money);

    // ── Left: Stat bars ──────────────────────────────────────
    CarStats cur  = CalcStats(car);
    CarStats prev = CalcPreviewStats(car, pi);

    DrawString(BAR_X, BAR_Y - 20, "Performance", GetColor(160, 160, 160));
    DrawStatBar(BAR_X, BAR_Y,      "Top Speed", cur.maxSpeed, prev.maxSpeed, 400.0f);
    DrawStatBar(BAR_X, BAR_Y + 35, "Accel    ", cur.accel,    prev.accel,      6.0f);

    // ── Right: Category tabs ─────────────────────────────────
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        SI_4 isSel = (i == s_categoryIndex);
        SI_4 isTab = (s_state == CUST_STATE_TAB);
        SI_4 col;
        if (isSel && isTab) col = GetColor(255, 255, 0);
        else if (isSel)     col = GetColor(200, 200, 100);
        else                col = GetColor(140, 140, 140);

        DrawFormatString(TAB_X + i * TAB_W, TAB_Y, col,
                         "%s%s", isSel ? "[" : " ", CAT_LABELS[i]);
        if (isSel) DrawFormatString(TAB_X + i * TAB_W + (SI_4)(strlen(CAT_LABELS[i]) * 8), TAB_Y, col, "]");
    }
    DrawLine(TAB_X - 5, TAB_Y + 18, 790, TAB_Y + 18, GetColor(80, 80, 80));

    // ── Right: Parts list ────────────────────────────────────
    SI_4 equippedPI = car->equippedParts[s_categoryIndex];

    for (int i = 0; i < s_filteredCount && i < LIST_MAX_SHOW; i++) {
        SI_4 listPI  = s_filteredList[i];
        SI_4 isSel   = (i == s_partsIndex);
        SI_4 isEquip = (listPI == equippedPI) || (listPI == -1 && equippedPI == -1);

        SI_4 col;
        if (isSel && s_state == CUST_STATE_LIST) col = GetColor(255, 255, 0);
        else if (isSel)                           col = GetColor(200, 200, 100);
        else                                      col = GetColor(160, 160, 160);

        if (listPI == -1) {
            DrawFormatString(LIST_X, LIST_Y + i * LIST_LINE_H, col,
                             "%s (none)%s",
                             isSel ? ">" : " ",
                             isEquip ? " [equipped]" : "");
        } else {
            SI_4 affordable = (g_gameData.money >= PARTS_TABLE[listPI].price);
            SI_4 nameCol = affordable ? col : GetColor(120, 80, 80);
            DrawFormatString(LIST_X, LIST_Y + i * LIST_LINE_H, nameCol,
                             "%s %-18s $%-5d%s",
                             isSel ? ">" : " ",
                             PARTS_TABLE[listPI].name,
                             PARTS_TABLE[listPI].price,
                             isEquip ? " [equipped]" : "");
        }
    }

    // ── Right: Part detail panel ─────────────────────────────
    DrawLine(TAB_X - 5, DETAIL_Y - 10, 790, DETAIL_Y - 10, GetColor(80, 80, 80));

    if (pi == -1) {
        DrawString(DETAIL_X, DETAIL_Y, "(none) - unequip current part", GetColor(160, 160, 160));
        DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE, "No cost / no refund", GetColor(120, 120, 120));
    } else {
        DrawFormatString(DETAIL_X, DETAIL_Y,
                         GetColor(255, 255, 255), "%s", PARTS_TABLE[pi].name);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 1,
                         GetColor(200, 200, 200),
                         "Top Speed  %+.0f km/h", PARTS_TABLE[pi].maxSpeedBonus);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 2,
                         GetColor(200, 200, 200),
                         "Accel      %+.2f", PARTS_TABLE[pi].accelBonus);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 3,
                         GetColor(255, 220, 80),
                         "Price      $%d", PARTS_TABLE[pi].price);

        SI_4 moneyCol = (g_gameData.money >= PARTS_TABLE[pi].price)
                        ? GetColor(255, 255, 255) : GetColor(255, 80, 80);
        DrawFormatString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 4, moneyCol,
                         "Wallet     $%d%s",
                         g_gameData.money,
                         (g_gameData.money >= PARTS_TABLE[pi].price) ? "" : "  [NOT ENOUGH]");
    }

    // Cannot-afford flash
    if (s_cantAfford > 0 && (s_cantAfford / 8) % 2 == 0) {
        DrawString(DETAIL_X, DETAIL_Y + DETAIL_LINE * 5,
                   "! Not enough money !", GetColor(255, 80, 80));
    }

    // ── Buttons ──────────────────────────────────────────────
    SI_4 showMenu = (s_state == CUST_STATE_MENU);
    SI_4 equipCol = (showMenu && s_cursorPos == 0) ? GetColor(255,255,0) : GetColor(130,130,130);
    SI_4 backCol  = (showMenu && s_cursorPos == 1) ? GetColor(255,255,0) : GetColor(130,130,130);

    if (showMenu) {
        if (s_cursorPos == 0)
            DrawBox(BTN_EQUIP_X - 4, BTN_Y - 3, BTN_EQUIP_X + 80, BTN_Y + 19,
                    GetColor(255,255,0), FALSE);
        else
            DrawBox(BTN_BACK_X - 4, BTN_Y - 3, BTN_BACK_X + 65, BTN_Y + 19,
                    GetColor(255,255,0), FALSE);
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
    DrawString(10, 578, hint, GetColor(80, 80, 80));
}
