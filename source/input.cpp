#include "../DxLib/DxLib.h"
#include "input.h"
#include <string.h>

static char s_prevKeys[256];
static char s_currKeys[256];

void UpdateInput(void) {
    memcpy(s_prevKeys, s_currKeys, sizeof(s_currKeys));
    GetHitKeyStateAll(s_currKeys);
}

int IsKeyTriggered(int keyCode) {
    return s_currKeys[keyCode] && !s_prevKeys[keyCode];
}

int IsKeyPressed(int keyCode) {
    return s_currKeys[keyCode];
}
