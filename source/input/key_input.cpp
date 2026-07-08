#include "key_input.h"
#include "../../DxLib/DxLib.h"
#include <string.h>

KeyInput::KeyInput() {
	memset(cPrevKeyState_, 0, sizeof(cPrevKeyState_));
	memset(cCurrKeyState_, 0, sizeof(cCurrKeyState_));
}

KeyInput::~KeyInput() {
}

void KeyInput::RefreshKeyState() {
	memcpy(cPrevKeyState_, cCurrKeyState_, sizeof(cCurrKeyState_));
	GetHitKeyStateAll(cCurrKeyState_);
}

bool KeyInput::IsPressed(const int iKeyCode) const {
	if (iKeyCode < 0 || iKeyCode >= KEY_INPUT_KEY_COUNT) {
		return false;
	}
	return cCurrKeyState_[iKeyCode] != 0;
}

bool KeyInput::IsTriggered(const int iKeyCode) const {
	if (iKeyCode < 0 || iKeyCode >= KEY_INPUT_KEY_COUNT) {
		return false;
	}
	return cCurrKeyState_[iKeyCode] != 0 && cPrevKeyState_[iKeyCode] == 0;
}
