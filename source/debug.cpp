#include "../DxLib/DxLib.h"
#include "../header/screen_manager.h"
#include "../header/input.h"
#include "../header/debug.h"
#include "../header/title.h"
#include <string.h>

#ifdef _DEBUG

// ─────────────────────────────────────────
// Debug Menu
// ─────────────────────────────────────────

static int s_debugMenuIndex = 0;

void UpdateDebugMenu(void) {
    if (IsKeyTriggered(KEY_INPUT_UP)) {
        s_debugMenuIndex = (s_debugMenuIndex - 1 + 2) % 2;
    }
    if (IsKeyTriggered(KEY_INPUT_DOWN)) {
        s_debugMenuIndex = (s_debugMenuIndex + 1) % 2;
    }
    if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
        if (s_debugMenuIndex == 0) ChangeScreen(UpdateDebugDraw,  DrawDebugDraw);
        else                       ChangeScreen(UpdateDebugInput, DrawDebugInput);
    }
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateTitle, DrawTitle);
    }
}

void DrawDebugMenu(void) {
    DrawString(300, 150, "DXLIB SAMPLE", GetColor(255, 255, 0));
    DrawLine(200, 200, 600, 200, GetColor(80, 80, 80));

    const char* items[] = { "Draw Sample", "Input Sample" };
    for (int i = 0; i < 2; i++) {
        int col = (i == s_debugMenuIndex) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawFormatString(320, 260 + i * 50, col, "%s %s",
                         (i == s_debugMenuIndex) ? ">" : " ", items[i]);
    }

    DrawString(250, 520, "[Z/Enter] Select  [ESC] Back to Title", GetColor(150, 150, 150));
}

// ─────────────────────────────────────────
// Draw Sample
// ─────────────────────────────────────────

#define DRAW_PAGE_COUNT 4
static int s_drawPage = 0;

static void DrawPageBasic(void) {
    int white = GetColor(255, 255, 255);
    int y = 70;

    // DrawPixel
    DrawString(40, y, "DrawPixel", white);
    for (int i = 0; i < 8; i++)
        DrawPixel(60 + i * 4, y + 45 + (i % 3), GetColor(255, 100, 100));

    // DrawLine
    DrawString(200, y, "DrawLine", white);
    DrawLine(210, y + 30, 310, y + 65, GetColor(100, 255, 100));

    // DrawBox
    DrawString(380, y, "DrawBox", white);
    DrawBox(385, y + 30, 490, y + 65, GetColor(100, 180, 255), FALSE);

    // DrawCircle
    DrawString(550, y, "DrawCircle", white);
    DrawCircle(590, y + 48, 28, GetColor(255, 200, 0), FALSE);

    y += 130;

    // DrawOval
    DrawString(40, y, "DrawOval", white);
    DrawOval(100, y + 45, 55, 25, GetColor(200, 100, 255), FALSE);

    // DrawTriangle
    DrawString(240, y, "DrawTriangle", white);
    DrawTriangle(260, y + 70, 310, y + 20, 360, y + 70, GetColor(0, 220, 200), FALSE);

    // DrawQuadrangle
    DrawString(420, y, "DrawQuadrangle", white);
    DrawQuadrangle(430, y + 70, 470, y + 20, 540, y + 35, 520, y + 70,
                   GetColor(255, 150, 0), FALSE);

    y += 130;

    // DrawRoundRect
    DrawString(40, y, "DrawRoundRect", white);
    DrawRoundRect(50, y + 25, 210, y + 70, 15, 15, GetColor(150, 255, 150), FALSE);
}

static void DrawPageFill(void) {
    int white = GetColor(255, 255, 255);
    int y = 70;

    DrawString(40,  y, "DrawBox (fill)",        white);
    DrawBox(40, y + 30, 180, y + 75, GetColor(100, 180, 255), TRUE);

    DrawString(240, y, "DrawCircle (fill)",      white);
    DrawCircle(295, y + 52, 30, GetColor(255, 200, 0), TRUE);

    DrawString(420, y, "DrawOval (fill)",        white);
    DrawOval(505, y + 52, 55, 25, GetColor(200, 100, 255), TRUE);

    y += 140;

    DrawString(40,  y, "DrawTriangle (fill)",    white);
    DrawTriangle(60, y + 70, 130, y + 20, 200, y + 70, GetColor(0, 220, 200), TRUE);

    DrawString(270, y, "DrawQuadrangle (fill)",  white);
    DrawQuadrangle(280, y + 70, 330, y + 20, 410, y + 38, 390, y + 70,
                   GetColor(255, 150, 0), TRUE);

    DrawString(490, y, "DrawRoundRect (fill)",   white);
    DrawRoundRect(500, y + 25, 660, y + 70, 15, 15, GetColor(150, 255, 150), TRUE);
}

static void DrawPageString(void) {
    int white = GetColor(255, 255, 255);
    int y = 70;

    DrawString(30, y, "DrawString", white);
    DrawString(30, y + 30, "Hello, DxLib!", GetColor(255, 255, 0));

    y += 90;
    DrawString(30, y, "DrawFormatString", white);
    DrawFormatString(30, y + 30, GetColor(100, 255, 100),
                     "int=%d  float=%.3f  hex=0x%X", 42, 3.14159f, 255);

    y += 90;
    DrawString(30, y, "GetDrawStringWidth", white);
    const char* sample = "SAMPLE TEXT";
    int w = GetDrawStringWidth(sample, (int)strlen(sample));
    DrawFormatString(30, y + 30, GetColor(255, 200, 0), "\"%s\"  width=%dpx", sample, w);
    DrawBox(30, y + 58, 30 + w, y + 64, GetColor(255, 100, 100), TRUE);

    y += 100;
    DrawString(30, y, "Color variations", white);
    int colors[] = {
        GetColor(255,   0,   0),
        GetColor(  0, 255,   0),
        GetColor(  0,   0, 255),
        GetColor(255, 255,   0),
        GetColor(255,   0, 255),
    };
    const char* labels[] = { "RED", "GREEN", "BLUE", "YELLOW", "MAGENTA" };
    for (int i = 0; i < 5; i++) {
        DrawString(30 + i * 140, y + 30, labels[i], colors[i]);
    }
}

static void DrawPageBlend(void) {
    // Colored background to make blend effects visible
    DrawFillBox(0, 50, 800, 450, GetColor(20, 50, 100));

    typedef struct { int mode; const char* name; } BlendEntry;
    BlendEntry blends[] = {
        { DX_BLENDMODE_NOBLEND, "NOBLEND"  },
        { DX_BLENDMODE_ALPHA,   "ALPHA"    },
        { DX_BLENDMODE_ADD,     "ADD"      },
        { DX_BLENDMODE_SUB,     "SUB"      },
        { DX_BLENDMODE_MUL,     "MUL"      },
    };
    int count = 5;

    for (int i = 0; i < count; i++) {
        int x = 30 + i * 150;
        int y = 70;
        DrawString(x, y, blends[i].name, GetColor(255, 255, 255));

        SetDrawBlendMode(blends[i].mode, 180);
        DrawFillBox(x, y + 25, x + 120, y + 135, GetColor(255, 100, 0));
        DrawCircle(x + 60, y + 195, 45, GetColor(0, 200, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    DrawString(20, 420, "alpha=180 applied to each mode", GetColor(160, 160, 160));
}

void UpdateDebugDraw(void) {
    if (IsKeyTriggered(KEY_INPUT_LEFT)) {
        s_drawPage = (s_drawPage - 1 + DRAW_PAGE_COUNT) % DRAW_PAGE_COUNT;
    }
    if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_drawPage = (s_drawPage + 1) % DRAW_PAGE_COUNT;
    }
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
    }
}

void DrawDebugDraw(void) {
    const char* pageNames[] = {
        "Basic Shapes",
        "Filled Shapes",
        "String Drawing",
        "Blend Modes",
    };

    DrawFormatString(10, 10, GetColor(255, 255, 0),
                     "Draw Sample  [</>] Page  [ESC] Back  (%d/%d) %s",
                     s_drawPage + 1, DRAW_PAGE_COUNT, pageNames[s_drawPage]);
    DrawLine(0, 35, 800, 35, GetColor(80, 80, 80));

    switch (s_drawPage) {
    case 0: DrawPageBasic();  break;
    case 1: DrawPageFill();   break;
    case 2: DrawPageString(); break;
    case 3: DrawPageBlend();  break;
    }
}

// ─────────────────────────────────────────
// Input Sample
// ─────────────────────────────────────────

typedef struct { int keyCode; const char* label; } KeyEntry;
static const KeyEntry KEY_LIST[] = {
    { KEY_INPUT_UP,     "KEY_INPUT_UP    " },
    { KEY_INPUT_DOWN,   "KEY_INPUT_DOWN  " },
    { KEY_INPUT_LEFT,   "KEY_INPUT_LEFT  " },
    { KEY_INPUT_RIGHT,  "KEY_INPUT_RIGHT " },
    { KEY_INPUT_Z,      "KEY_INPUT_Z     " },
    { KEY_INPUT_X,      "KEY_INPUT_X     " },
    { KEY_INPUT_A,      "KEY_INPUT_A     " },
    { KEY_INPUT_D,      "KEY_INPUT_D     " },
    { KEY_INPUT_RETURN, "KEY_INPUT_RETURN" },
    { KEY_INPUT_ESCAPE, "KEY_INPUT_ESCAPE" },
    { KEY_INPUT_F1,     "KEY_INPUT_F1    " },
};
#define KEY_LIST_COUNT ((int)(sizeof(KEY_LIST) / sizeof(KEY_LIST[0])))

typedef struct { int padBit; const char* label; } PadEntry;
static const PadEntry PAD_LIST[] = {
    { PAD_INPUT_UP,    "PAD_INPUT_UP   " },
    { PAD_INPUT_DOWN,  "PAD_INPUT_DOWN " },
    { PAD_INPUT_LEFT,  "PAD_INPUT_LEFT " },
    { PAD_INPUT_RIGHT, "PAD_INPUT_RIGHT" },
    { PAD_INPUT_1,     "PAD_INPUT_1    " },
    { PAD_INPUT_2,     "PAD_INPUT_2    " },
    { PAD_INPUT_3,     "PAD_INPUT_3    " },
    { PAD_INPUT_4,     "PAD_INPUT_4    " },
};
#define PAD_LIST_COUNT ((int)(sizeof(PAD_LIST) / sizeof(PAD_LIST[0])))

static int s_lastKeyIndex = -1;

void UpdateDebugInput(void) {
    for (int i = 0; i < KEY_LIST_COUNT; i++) {
        if (IsKeyTriggered(KEY_LIST[i].keyCode)) {
            s_lastKeyIndex = i;
        }
    }
    // ESC transitions back (captured after updating lastKey)
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        s_lastKeyIndex = -1;
        ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
    }
}

void DrawDebugInput(void) {
    DrawString(10, 10, "Input Sample  [ESC] Back", GetColor(255, 255, 0));
    DrawLine(0,   35, 800,  35, GetColor(80, 80, 80));
    DrawLine(400, 35, 400, 590, GetColor(80, 80, 80));

    // ── Left: Keyboard ──────────────────────────────
    DrawString(30, 45, "Keyboard", GetColor(200, 200, 200));

    for (int i = 0; i < KEY_LIST_COUNT; i++) {
        int isOn = IsKeyPressed(KEY_LIST[i].keyCode);
        int col  = isOn ? GetColor(255, 255, 0) : GetColor(160, 160, 160);
        DrawFormatString(30, 70 + i * 26, col, "%s : %s",
                         KEY_LIST[i].label, isOn ? "[ON] " : "[   ]");
    }

    DrawString(30, 375, "Last triggered:", GetColor(200, 200, 200));
    if (s_lastKeyIndex >= 0) {
        DrawString(30, 398, KEY_LIST[s_lastKeyIndex].label, GetColor(255, 220, 0));
    } else {
        DrawString(30, 398, "---", GetColor(100, 100, 100));
    }

    // ── Right: Controller ───────────────────────────
    DrawString(420, 45, "Controller (PAD1)", GetColor(200, 200, 200));

    int padInput    = GetJoypadInputState(DX_INPUT_PAD1);
    int isConnected = (GetJoypadNum() > 0);

    for (int i = 0; i < PAD_LIST_COUNT; i++) {
        int isOn = isConnected && (padInput & PAD_LIST[i].padBit);
        int col  = isOn ? GetColor(255, 255, 0) : GetColor(160, 160, 160);
        DrawFormatString(420, 70 + i * 26, col, "%s : %s",
                         PAD_LIST[i].label, isOn ? "[ON] " : "[   ]");
    }

    // Analog sticks
    DrawString(420, 295, "Analog Stick", GetColor(200, 200, 200));
    if (isConnected) {
        int lx = 0, ly = 0, rx = 0, ry = 0;
        GetJoypadAnalogInput(&lx, &ly, DX_INPUT_PAD1);
        //GetJoypadAnalogInput2(&rx, &ry, DX_INPUT_PAD1);
        DrawFormatString(420, 318, GetColor(255, 255, 255),
                         "Left  X:%+6.3f  Y:%+6.3f", lx / 1000.0f, ly / 1000.0f);
        //DrawFormatString(420, 341, GetColor(255, 255, 255),
        //                 "Right X:%+6.3f  Y:%+6.3f", rx / 1000.0f, ry / 1000.0f);
    } else {
        DrawString(420, 318, "N/A (not connected)", GetColor(100, 100, 100));
    }

    // Connection status
    DrawFormatString(420, 390,
                     isConnected ? GetColor(100, 255, 100) : GetColor(255, 100, 100),
                     "Status: %s", isConnected ? "Connected" : "Not connected");
}

#endif // _DEBUG
