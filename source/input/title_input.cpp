#include "title_input.h"
#include "../../DxLib/DxLib.h"

TitleInput::TitleInput()
	: bIsAnyInputPressed_(false)
	, bIsDebugMenuTriggered_(false)
	, bIsMeterTriggered_(false)
{
}

void TitleInput::Update()
{
	RefreshKeyState();

	// "any input" also accepts pad / mouse, same as the old direct CheckHitKeyAll call
	bIsAnyInputPressed_    = (CheckHitKeyAll(DX_CHECKINPUT_ALL) != 0);
	bIsDebugMenuTriggered_ = IsTriggered(KEY_INPUT_F1);
	bIsMeterTriggered_     = IsTriggered(KEY_INPUT_F2);
}

bool TitleInput::IsAnyInputPressed() const
{
	return bIsAnyInputPressed_;
}

bool TitleInput::IsDebugMenuTriggered() const
{
	return bIsDebugMenuTriggered_;
}

bool TitleInput::IsMeterTriggered() const
{
	return bIsMeterTriggered_;
}
