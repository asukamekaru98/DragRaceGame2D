#include "result_input.h"
#include "../../DxLib/DxLib.h"

ResultInput::ResultInput()
	: bIsAnyInputPressed_(false)
{
}

void ResultInput::Update()
{
	RefreshKeyState();

	// "any input" also accepts pad / mouse, same as the old direct CheckHitKeyAll call
	bIsAnyInputPressed_ = (CheckHitKeyAll(DX_CHECKINPUT_ALL) != 0);
}

bool ResultInput::IsAnyInputPressed() const
{
	return bIsAnyInputPressed_;
}
