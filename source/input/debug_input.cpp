#include "debug_input.h"

#ifdef _DEBUG

#include "../../DxLib/DxLib.h"

DebugInput::DebugInput()
	: bIsAccel_(false)
	, bIsGearUpTriggered_(false)
	, bIsGearDownTriggered_(false)
	, bIsRetireTriggered_(false)
{
}

void DebugInput::Update()
{
	RefreshKeyState();

	// spec/detail_design_debug.md 5.2
	bIsAccel_             = IsPressed(KEY_INPUT_Z);
	bIsGearUpTriggered_   = IsTriggered(KEY_INPUT_RIGHT);
	bIsGearDownTriggered_ = IsTriggered(KEY_INPUT_LEFT);
	bIsRetireTriggered_   = IsTriggered(KEY_INPUT_ESCAPE);
}

bool DebugInput::IsAccel() const
{
	return bIsAccel_;
}

bool DebugInput::IsGearUpTriggered() const
{
	return bIsGearUpTriggered_;
}

bool DebugInput::IsGearDownTriggered() const
{
	return bIsGearDownTriggered_;
}

bool DebugInput::IsRetireTriggered() const
{
	return bIsRetireTriggered_;
}

#endif // _DEBUG
