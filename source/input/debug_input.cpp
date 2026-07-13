#include "debug_input.h"

#ifdef _DEBUG

#include "../../DxLib/DxLib.h"

DebugInput::DebugInput()
	: bIsAccel_(false)
	, bIsGearUpTriggered_(false)
	, bIsGearDownTriggered_(false)
	, bIsRetireTriggered_(false)
	, bIsResetTriggered_(false)
	, iCarSelectTriggered_(-1)
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
	bIsResetTriggered_    = IsTriggered(KEY_INPUT_R);

	static const int CAR_SELECT_KEYS[DEBUG_INPUT_CAR_SELECT_COUNT] = {
		KEY_INPUT_1, KEY_INPUT_2, KEY_INPUT_3, KEY_INPUT_4, KEY_INPUT_5,
	};
	iCarSelectTriggered_ = -1;
	for (int i = 0; i < DEBUG_INPUT_CAR_SELECT_COUNT; i++) {
		if (IsTriggered(CAR_SELECT_KEYS[i])) {
			iCarSelectTriggered_ = i;
		}
	}
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

bool DebugInput::IsResetTriggered() const
{
	return bIsResetTriggered_;
}

int DebugInput::GetCarSelectTriggered() const
{
	return iCarSelectTriggered_;
}

DebugRawInput::DebugRawInput()
{
}

void DebugRawInput::Update()
{
	RefreshKeyState();
}

bool DebugRawInput::IsKeyPressed(const int iKeyCode) const
{
	return IsPressed(iKeyCode);
}

bool DebugRawInput::IsKeyTriggered(const int iKeyCode) const
{
	return IsTriggered(iKeyCode);
}

#endif // _DEBUG
