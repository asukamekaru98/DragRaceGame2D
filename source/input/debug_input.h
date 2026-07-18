#pragma once

#ifdef _DEBUG

#include "key_input.h"

// Thin wrapper that exposes raw key queries for the debug sample screens
class DebugRawInput :public KeyInput {
public:
	DebugRawInput();
	void Update() override;
	bool IsKeyPressed(const int iKeyCode) const;
	bool IsKeyTriggered(const int iKeyCode) const;
};

#endif // _DEBUG
