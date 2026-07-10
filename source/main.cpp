#include "../DxLib/DxLib.h"
#include "typedef.h"
#include "main.h"
#include "screen_manager.h"
#include "input.h"
#include "title_resource.h"
#include "opening.h"
#include "debug.h"
#include "game_data.h"
#include "const.h"

static INIT_RESULT InitProgram(void) {
    ChangeWindowMode(TRUE);
    SetMainWindowText(WINDOW_SCREEN_NAME);
    SetGraphMode(SCREEN_SIZE_WIDTH, SCREEN_SIZE_HEIGHT, COLOR_BIT_DEPTH);
    SetWindowSizeChangeEnableFlag(FALSE);
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

    if (DxLib_Init() == ERROR) {
        throw INIT_RESULT_ERROR;
    }

    SetDrawScreen(DX_SCREEN_BACK);
    return INIT_RESULT_SUCCESS;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    try {
        InitProgram();
    }
    catch (INIT_RESULT e) {
        return 0;
    }

    InitGameData();

    TitleResource titleResource;
    titleResource.LoadResources();

# if 1
	g_screen.updateInput = g_ScreenFuncs[SCREEN_OPENING].updateInput;
    g_screen.update     = g_ScreenFuncs[SCREEN_OPENING].update;
    g_screen.draw       = g_ScreenFuncs[SCREEN_OPENING].draw;
    g_screen.nextUpdate = NULL;
    g_screen.nextDraw   = NULL;
#else

    g_screen.update = UpdateDebugMenu;
    g_screen.draw = DrawDebugMenu;
    g_screen.nextUpdate = NULL;
    g_screen.nextDraw = NULL;
#endif

    while (ProcessMessage() == SUCCESS) {
        ClearDrawScreen();

        UpdateInput();
        g_screen.update();
        g_screen.draw();

        if (g_screen.nextUpdate != NULL) {
            g_screen.update     = g_screen.nextUpdate;
            g_screen.draw       = g_screen.nextDraw;
            g_screen.nextUpdate = NULL;
            g_screen.nextDraw   = NULL;
        }

        ScreenFlip();
    }

    titleResource.UnloadResources();
    DxLib_End();
    return 0;
}

// マージテスト1 main
