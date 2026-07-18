#include "../DxLib/DxLib.h"
#include "share.h"
#include "screen_manager.h"
#include "input/debug_input.h"
#include "debug.h"
#include "title.h"
#include "car_data.h"
#include "car_engine/car_engine_data.h"
#include "car_meter/car_meter.h"
#include <string.h>
#include <math.h>

#ifdef _DEBUG

// Raw key input shared by the debug menu / draw sample / input sample screens
static DebugRawInput s_rawInput;

void UpdateDebugRawInput(void) {
    s_rawInput.Update();
}

// ─────────────────────────────────────────
// Debug Menu
// ─────────────────────────────────────────

static int s_debugMenuIndex = 0;

void UpdateDebugMenu(void) {
    static int s_initialized = 0;
    if (!s_initialized) {
        s_rawInput.Update();   // re-sync: suppress false triggers from keys held across screens
        s_initialized = 1;
    }

    if (s_rawInput.IsKeyTriggered(KEY_INPUT_UP)) {
        s_debugMenuIndex = (s_debugMenuIndex - 1 + 2) % 2;
    }
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_DOWN)) {
        s_debugMenuIndex = (s_debugMenuIndex + 1) % 2;
    }
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_Z) || s_rawInput.IsKeyTriggered(KEY_INPUT_RETURN)) {
        s_initialized = 0;
        if (s_debugMenuIndex == 0) ChangeScreen(UpdateDebugRawInput, UpdateDebugDraw,  DrawDebugDraw);
        else                       ChangeScreen(UpdateDebugRawInput, UpdateDebugInput, DrawDebugInput);
    }
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        s_initialized = 0;
        ChangeScreen(UpdateTitleInput, UpdateTitle, DrawTitle);
    }
}

void DrawDebugMenu(void) {
    DrawString(300, 150, "DXLIB SAMPLE", Color::YELLOW.Code());
    DrawLine(200, 200, 600, 200, Color(80, 80, 80).Code());

    const char* items[] = { "Draw Sample", "Input Sample" };
    for (int i = 0; i < 2; i++) {
        int col = (i == s_debugMenuIndex) ? Color::YELLOW.Code() : Color(200, 200, 200).Code();
        DrawFormatString(320, 260 + i * 50, col, "%s %s",
                         (i == s_debugMenuIndex) ? ">" : " ", items[i]);
    }

    DrawString(250, 520, "[Z/Enter] Select  [ESC] Back to Title", Color(150, 150, 150).Code());
}

// ─────────────────────────────────────────
// Draw Sample
// ─────────────────────────────────────────

#define DRAW_PAGE_COUNT 4
static int s_drawPage = 0;

static void DrawPageBasic(void) {
    int white = Color::WHITE.Code();
    int y = 70;

    // DrawPixel
    DrawString(40, y, "DrawPixel", white);
    for (int i = 0; i < 8; i++)
        DrawPixel(60 + i * 4, y + 45 + (i % 3), Color(255, 100, 100).Code());

    // DrawLine
    DrawString(200, y, "DrawLine", white);
    DrawLine(210, y + 30, 310, y + 65, Color(100, 255, 100).Code());

    // DrawBox
    DrawString(380, y, "DrawBox", white);
    DrawBox(385, y + 30, 490, y + 65, Color(100, 180, 255).Code(), FALSE);

    // DrawCircle
    DrawString(550, y, "DrawCircle", white);
    DrawCircle(590, y + 48, 28, Color(255, 200, 0).Code(), FALSE);

    y += 130;

    // DrawOval
    DrawString(40, y, "DrawOval", white);
    DrawOval(100, y + 45, 55, 25, Color(200, 100, 255).Code(), FALSE);

    // DrawTriangle
    DrawString(240, y, "DrawTriangle", white);
    DrawTriangle(260, y + 70, 310, y + 20, 360, y + 70, Color(0, 220, 200).Code(), FALSE);

    // DrawQuadrangle
    DrawString(420, y, "DrawQuadrangle", white);
    DrawQuadrangle(430, y + 70, 470, y + 20, 540, y + 35, 520, y + 70,
                   Color(255, 150, 0).Code(), FALSE);

    y += 130;

    // DrawRoundRect
    DrawString(40, y, "DrawRoundRect", white);
    DrawRoundRect(50, y + 25, 210, y + 70, 15, 15, Color(150, 255, 150).Code(), FALSE);
}

static void DrawPageFill(void) {
    int white = Color::WHITE.Code();
    int y = 70;

    DrawString(40,  y, "DrawBox (fill)",        white);
    DrawBox(40, y + 30, 180, y + 75, Color(100, 180, 255).Code(), TRUE);

    DrawString(240, y, "DrawCircle (fill)",      white);
    DrawCircle(295, y + 52, 30, Color(255, 200, 0).Code(), TRUE);

    DrawString(420, y, "DrawOval (fill)",        white);
    DrawOval(505, y + 52, 55, 25, Color(200, 100, 255).Code(), TRUE);

    y += 140;

    DrawString(40,  y, "DrawTriangle (fill)",    white);
    DrawTriangle(60, y + 70, 130, y + 20, 200, y + 70, Color(0, 220, 200).Code(), TRUE);

    DrawString(270, y, "DrawQuadrangle (fill)",  white);
    DrawQuadrangle(280, y + 70, 330, y + 20, 410, y + 38, 390, y + 70,
                   Color(255, 150, 0).Code(), TRUE);

    DrawString(490, y, "DrawRoundRect (fill)",   white);
    DrawRoundRect(500, y + 25, 660, y + 70, 15, 15, Color(150, 255, 150).Code(), TRUE);
}

static void DrawPageString(void) {
    int white = Color::WHITE.Code();
    int y = 70;

    DrawString(30, y, "DrawString", white);
    DrawString(30, y + 30, "Hello, DxLib!", Color::YELLOW.Code());

    y += 90;
    DrawString(30, y, "DrawFormatString", white);
    DrawFormatString(30, y + 30, Color(100, 255, 100).Code(),
                     "int=%d  float=%.3f  hex=0x%X", 42, 3.14159f, 255);

    y += 90;
    DrawString(30, y, "GetDrawStringWidth", white);
    const char* sample = "SAMPLE TEXT";
    int w = GetDrawStringWidth(sample, (int)strlen(sample));
    DrawFormatString(30, y + 30, Color(255, 200, 0).Code(), "\"%s\"  width=%dpx", sample, w);
    DrawBox(30, y + 58, 30 + w, y + 64, Color(255, 100, 100).Code(), TRUE);

    y += 100;
    DrawString(30, y, "Color variations", white);
    int colors[] = {
        Color::RED.Code(),
        Color::GREEN.Code(),
        Color::BLUE.Code(),
        Color::YELLOW.Code(),
        Color::MAGENTA.Code(),
    };
    const char* labels[] = { "RED", "GREEN", "BLUE", "YELLOW", "MAGENTA" };
    for (int i = 0; i < 5; i++) {
        DrawString(30 + i * 140, y + 30, labels[i], colors[i]);
    }
}

static void DrawPageBlend(void) {
    // Colored background to make blend effects visible
    DrawFillBox(0, 50, 800, 450, Color(20, 50, 100).Code());

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
        DrawString(x, y, blends[i].name, Color::WHITE.Code());

        SetDrawBlendMode(blends[i].mode, 180);
        DrawFillBox(x, y + 25, x + 120, y + 135, Color(255, 100, 0).Code());
        DrawCircle(x + 60, y + 195, 45, Color(0, 200, 255).Code(), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    DrawString(20, 420, "alpha=180 applied to each mode", Color(160, 160, 160).Code());
}

void UpdateDebugDraw(void) {
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_LEFT)) {
        s_drawPage = (s_drawPage - 1 + DRAW_PAGE_COUNT) % DRAW_PAGE_COUNT;
    }
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_drawPage = (s_drawPage + 1) % DRAW_PAGE_COUNT;
    }
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateDebugRawInput, UpdateDebugMenu, DrawDebugMenu);
    }
}

void DrawDebugDraw(void) {
    const char* pageNames[] = {
        "Basic Shapes",
        "Filled Shapes",
        "String Drawing",
        "Blend Modes",
    };

    DrawFormatString(10, 10, Color::YELLOW.Code(),
                     "Draw Sample  [</>] Page  [ESC] Back  (%d/%d) %s",
                     s_drawPage + 1, DRAW_PAGE_COUNT, pageNames[s_drawPage]);
    DrawLine(0, 35, 800, 35, Color(80, 80, 80).Code());

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
        if (s_rawInput.IsKeyTriggered(KEY_LIST[i].keyCode)) {
            s_lastKeyIndex = i;
        }
    }
    // ESC transitions back (captured after updating lastKey)
    if (s_rawInput.IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        s_lastKeyIndex = -1;
        ChangeScreen(UpdateDebugRawInput, UpdateDebugMenu, DrawDebugMenu);
    }
}

void DrawDebugInput(void) {
    DrawString(10, 10, "Input Sample  [ESC] Back", Color::YELLOW.Code());
    DrawLine(0,   35, 800,  35, Color(80, 80, 80).Code());
    DrawLine(400, 35, 400, 590, Color(80, 80, 80).Code());

    // ── Left: Keyboard ──────────────────────────────
    DrawString(30, 45, "Keyboard", Color(200, 200, 200).Code());

    for (int i = 0; i < KEY_LIST_COUNT; i++) {
        int isOn = s_rawInput.IsKeyPressed(KEY_LIST[i].keyCode);
        int col  = isOn ? Color::YELLOW.Code() : Color(160, 160, 160).Code();
        DrawFormatString(30, 70 + i * 26, col, "%s : %s",
                         KEY_LIST[i].label, isOn ? "[ON] " : "[   ]");
    }

    DrawString(30, 375, "Last triggered:", Color(200, 200, 200).Code());
    if (s_lastKeyIndex >= 0) {
        DrawString(30, 398, KEY_LIST[s_lastKeyIndex].label, Color(255, 220, 0).Code());
    } else {
        DrawString(30, 398, "---", Color(100, 100, 100).Code());
    }

    // ── Right: Controller ───────────────────────────
    DrawString(420, 45, "Controller (PAD1)", Color(200, 200, 200).Code());

    int padInput    = GetJoypadInputState(DX_INPUT_PAD1);
    int isConnected = (GetJoypadNum() > 0);

    for (int i = 0; i < PAD_LIST_COUNT; i++) {
        int isOn = isConnected && (padInput & PAD_LIST[i].padBit);
        int col  = isOn ? Color::YELLOW.Code() : Color(160, 160, 160).Code();
        DrawFormatString(420, 70 + i * 26, col, "%s : %s",
                         PAD_LIST[i].label, isOn ? "[ON] " : "[   ]");
    }

    // Analog sticks
    DrawString(420, 295, "Analog Stick", Color(200, 200, 200).Code());
    if (isConnected) {
        int lx = 0, ly = 0, rx = 0, ry = 0;
        GetJoypadAnalogInput(&lx, &ly, DX_INPUT_PAD1);
        //GetJoypadAnalogInput2(&rx, &ry, DX_INPUT_PAD1);
        DrawFormatString(420, 318, Color::WHITE.Code(),
                         "Left  X:%+6.3f  Y:%+6.3f", lx / 1000.0f, ly / 1000.0f);
        //DrawFormatString(420, 341, Color::WHITE.Code(),
        //                 "Right X:%+6.3f  Y:%+6.3f", rx / 1000.0f, ry / 1000.0f);
    } else {
        DrawString(420, 318, "N/A (not connected)", Color(100, 100, 100).Code());
    }

    // Connection status
    DrawFormatString(420, 390,
                     isConnected ? Color(100, 255, 100).Code() : Color(255, 100, 100).Code(),
                     "Status: %s", isConnected ? "Connected" : "Not connected");
}

// ─────────────────────────────────────────
// Car Meter (tachometer / speed) prototype
//   A standalone screen for tuning the meter needle animation and behaviour.
// ─────────────────────────────────────────

#define MTR_RPM_IDLE     800.0f
#define MTR_RPM_REDLINE  6800.0f
#define MTR_RPM_MAX      8000.0f
#define MTR_SMOOTH       0.15f      // needle easing (0..1, smaller = smoother/slower)
#define MTR_TACH_START   210.0f     // needle angle (deg) at rpm 0
#define MTR_TACH_SWEEP   240.0f     // total sweep (deg)

static int   s_meterInit = 0;       // init-on-entry flag
static int   s_meterCar  = 0;       // test car index into CAR_TABLE
static float s_rpm       = MTR_RPM_IDLE;   // target engine rpm
static int   s_gear      = 1;
static float s_speed     = 0.0f;
static DebugInput s_meterInput;     // throttle / gear / car-select / reset / back
static CarMeter   s_meter;          // gauge under test (owns needle easing + drawing)

static void ResetMeter(void) {
    s_rpm   = MTR_RPM_IDLE;
    s_gear  = 1;
    s_speed = 0.0f;
    s_meter.Reset();
}

void UpdateDebugMeterInput(void) {
    s_meterInput.Update();
}

void UpdateDebugMeter(void) {
    if (!s_meterInit) {
        ResetMeter();
        s_meterInput.Update();   // re-sync: suppress false triggers from keys held across screens
        s_meterInit = 1;
    }

    const CarData* car = &CAR_TABLE[s_meterCar];

    // Throttle (hold Z): rev up with a rev-limiter, else engine-brake down to idle
    if (s_meterInput.IsAccel()) {
        s_rpm += car->acceleration * 40.0f;
        if (s_rpm > MTR_RPM_REDLINE) s_rpm = MTR_RPM_REDLINE;
    } else {
        s_rpm -= 90.0f;
        if (s_rpm < MTR_RPM_IDLE) s_rpm = MTR_RPM_IDLE;
    }

    // Gear shift: upshift drops rpm, downshift raises it
    if (s_meterInput.IsGearUpTriggered() && s_gear < car->gearCount) {
        s_gear++;
        s_rpm *= 0.6f;
        if (s_rpm < MTR_RPM_IDLE) s_rpm = MTR_RPM_IDLE;
    }
    if (s_meterInput.IsGearDownTriggered() && s_gear > 1) {
        s_gear--;
        s_rpm *= 1.5f;
        if (s_rpm > MTR_RPM_REDLINE) s_rpm = MTR_RPM_REDLINE;
    }

    // Switch test car with number keys 1..5
    {
        int carSelect = s_meterInput.GetCarSelectTriggered();
        if (carSelect >= 0 && carSelect < CAR_TABLE_COUNT) {
            s_meterCar = carSelect;
            ResetMeter();
        }
    }

    if (s_meterInput.IsResetTriggered()) ResetMeter();

    // Speed from rpm and current gear (top gear at redline ~ car maxSpeed)
    float gearTop = car->maxSpeed * ((float)s_gear / (float)car->gearCount);
    float revFrac = (s_rpm - MTR_RPM_IDLE) / (MTR_RPM_REDLINE - MTR_RPM_IDLE);
    if (revFrac < 0.0f) revFrac = 0.0f;
    s_speed = gearTop * revFrac;

    // Hand the engine state to the meter; it owns the needle easing / drawing
    CarEngineData meterData = { s_rpm, s_speed, s_gear, 0.0f, false };
    s_meter.Update(meterData);

    if (s_meterInput.IsRetireTriggered()) {
        s_meterInit = 0;
        ChangeScreen(UpdateTitleInput, UpdateTitle, DrawTitle);
    }
}

void DrawDebugMeter(void) {
    const CarData* car = &CAR_TABLE[s_meterCar];

    DrawFormatString(10, 10, Color::YELLOW.Code(),
                     "Car Meter (prototype)   Car[%d]: %s   [ESC] Back",
                     s_meterCar + 1, car->name);
    DrawLine(0, 35, 800, 35, Color(80, 80, 80).Code());

    // Gauge cluster (dial + eased needle + RPM/SPEED/GEAR readouts)
    s_meter.Draw();

    // Right panel: harness-only chrome (throttle state + speed bar)
    int px2 = 520, py2 = 110;

    DrawString(px2, py2 + 150, "THROTTLE", Color(150, 150, 160).Code());
    DrawFillBox(px2 + 90, py2 + 150, px2 + 112, py2 + 168,
                s_meterInput.IsAccel() ? Color(0, 220, 0).Code() : Color(60, 60, 60).Code());

    // speed bar
    int barX = px2, barY = py2 + 210, barW = 230, barH = 16;
    DrawBox(barX, barY, barX + barW, barY + barH, Color(100, 100, 100).Code(), FALSE);
    float sp = (car->maxSpeed > 0.0f) ? s_speed / car->maxSpeed : 0.0f;
    if (sp > 1.0f) sp = 1.0f;
    DrawFillBox(barX + 1, barY + 1, barX + 1 + (int)((barW - 2) * sp), barY + barH - 1,
                Color(80, 160, 255).Code());

    DrawString(20, 565,
               "[Z] Throttle   [<-/->] Shift Down/Up   [1-5] Car   [R] Reset   [ESC] Back",
               Color(150, 150, 150).Code());
}

#endif // _DEBUG
