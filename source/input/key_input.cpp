#include "key_input.h"

void KeyInput::Update() {
	memcpy(s_prevKeys, s_currKeys, sizeof(s_currKeys));
	GetHitKeyStateAll(s_currKeys);
}

void KeyInput::RefreshKeyState() {

}

bool KeyInput::IsPressed(int keyCode) {
	return false;
}

bool KeyInput::IsTriggered(int keyCode) {
	return false;
}