#pragma once

#include "key_input.h"

class DebugInput :public KeyInput {
	void RefreshKeyState();
};