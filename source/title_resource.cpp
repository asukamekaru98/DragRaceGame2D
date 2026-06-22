#include "../DxLib/DxLib.h"
#include "../header/title_resource.h"

// タイトル/オープニングで共有する画像ハンドルの実体。
// 読み込み・解放は TitleResource クラス（基底 ResourceManager）が行う。
int g_hBg      = -1;
int g_hShutter = -1;
int g_hLogo    = -1;
