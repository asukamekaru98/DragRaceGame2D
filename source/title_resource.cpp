#include "../DxLib/DxLib.h"
#include "../header/title_resource.h"

int g_hBg      = -1;
int g_hShutter = -1;
int g_hLogo    = -1;

void LoadTitleResource(void) {
    g_hBg      = LoadGraph("resource/title/tite_sprite1.png");
    g_hShutter = LoadGraph("resource/title/tite_sprite2.png");
    g_hLogo    = LoadGraph("resource/logo.png");
}

void UnloadTitleResource(void) {
    DeleteGraph(g_hBg);
    DeleteGraph(g_hShutter);
    DeleteGraph(g_hLogo);
    g_hBg = g_hShutter = g_hLogo = -1;
}
