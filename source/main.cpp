#include "../DxLib/DxLib.h"
#include "../header/typedef.h"
#include "../header/main.h"
#include <math.h>



INIT_RESULT InitProgram() {

    // All window / graphics settings must be configured BEFORE DxLib_Init.
    ChangeWindowMode(TRUE);                        // run in windowed mode
    SetMainWindowText(WINDOW_SCREEN_NAME);         // window title
    SetGraphMode(800, 600, 32);                    // drawing resolution 800x600 (32bit)
    SetWindowSizeChangeEnableFlag(FALSE);          // disable manual window resizing
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);  // string char code format

    if (DxLib_Init() == -1) {
        throw INIT_RESULT_ERROR;                   // failed to initialize DX library
    }

    SetDrawScreen(DX_SCREEN_BACK);                 // draw to back buffer (double buffering)

    return INIT_RESULT_SUCCESS;
}


// HSV(Hue:0-360, S/V:0-1) -> DxLib color. Used to sweep through the rainbow.
static int HsvToColor(double h, double s, double v) {
    double c = v * s;
    double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;
    double r = 0.0, g = 0.0, b = 0.0;

    if      (h <  60.0) { r = c; g = x; b = 0; }
    else if (h < 120.0) { r = x; g = c; b = 0; }
    else if (h < 180.0) { r = 0; g = c; b = x; }
    else if (h < 240.0) { r = 0; g = x; b = c; }
    else if (h < 300.0) { r = x; g = 0; b = c; }
    else                { r = c; g = 0; b = x; }

    int ri = (int)((r + m) * 255.0 + 0.5);
    int gi = (int)((g + m) * 255.0 + 0.5);
    int bi = (int)((b + m) * 255.0 + 0.5);
    return GetColor(ri, gi, bi);
}


OPENING_WINDOW Opening() {

    static double hue = 0.0; // current hue, advances every frame

    int width = 0, height = 0;
    GetDrawScreenSize(&width, &height);

    // paint the whole screen with the current rainbow color
    DrawFillBox(0, 0, width, height, HsvToColor(hue, 1.0, 1.0));

    hue += 1.0;            // sweep speed (degrees per frame)
    if (hue >= 360.0) {
        hue -= 360.0;
    }

    return OP_SUCCESS;
}


int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
   
    
    try {
        InitProgram();



    }
    catch (INIT_RESULT exception) {


        return 0;
    };
        

    // main loop: keep running until the window is closed or ESC is pressed
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        ClearDrawScreen();

        if (OP_FAILURE == Opening()) {
            break;
        }

        ScreenFlip();
    }

   



    DxLib_End();    // DXÉâÉCÉuÉâÉäèIóπèàóù
    return 0;
}
