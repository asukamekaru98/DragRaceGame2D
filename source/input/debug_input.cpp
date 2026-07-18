#include "debug_input.h"

#ifdef _DEBUG

#include "../../DxLib/DxLib.h"

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
