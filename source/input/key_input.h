#pragma once
#include "../../DxLib/DxLib.h"
#include <string.h>

#define KEY_INPUT_KEY_COUNT 256

class KeyInput {	// abstract
public:
	virtual void Update();

private:
	char s_prevKeys[KEY_INPUT_KEY_COUNT];
	char s_currKeys[KEY_INPUT_KEY_COUNT];
	bool bPrevKetState_[KEY_INPUT_KEY_COUNT];
	bool bCurrKetState_[KEY_INPUT_KEY_COUNT];

protected:
	void RefreshKeyState();
	bool IsPressed(const int iKeyCode);
	bool IsTriggered(const int iKeyCode);
};