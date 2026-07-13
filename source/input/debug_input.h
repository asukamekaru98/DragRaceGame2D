#pragma once

#ifdef _DEBUG

#include "key_input.h"

#define DEBUG_INPUT_CAR_SELECT_COUNT 5

class DebugInput :public KeyInput {
private:
	bool bIsAccel_;
	bool bIsGearUpTriggered_;
	bool bIsGearDownTriggered_;
	bool bIsRetireTriggered_;
	bool bIsResetTriggered_;
	int  iCarSelectTriggered_;	// 0..4 when a number key (1-5) is triggered, -1 otherwise

public:
	DebugInput();
	void Update() override;
	bool IsAccel() const;
	bool IsGearUpTriggered() const;
	bool IsGearDownTriggered() const;
	bool IsRetireTriggered() const;
	bool IsResetTriggered() const;
	int  GetCarSelectTriggered() const;
};

// Thin wrapper that exposes raw key queries for the debug sample screens
class DebugRawInput :public KeyInput {
public:
	DebugRawInput();
	void Update() override;
	bool IsKeyPressed(const int iKeyCode) const;
	bool IsKeyTriggered(const int iKeyCode) const;
};

#endif // _DEBUG
