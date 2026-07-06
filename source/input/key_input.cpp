#include "key_input.h"

void KeyInput::Update() {
	memcpy(s_prevKeys, s_currKeys, sizeof(s_currKeys));
	GetHitKeyStateAll(s_currKeys);
}

void KeyInput::RefreshKeyState() const {

}

bool KeyInput::IsPressed(int keyCode) const {
	return false;
}

bool KeyInput::IsTriggered(int keyCode) const {
	return false;
}