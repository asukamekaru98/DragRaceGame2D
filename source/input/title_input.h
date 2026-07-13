#pragma once

#include "key_input.h"

// Key input for the title screen
class TitleInput : public KeyInput {
private:
	bool bIsAnyInputPressed_;
	bool bIsDebugMenuTriggered_;
	bool bIsMeterTriggered_;

public:
	TitleInput();
	void Update() override;
	bool IsAnyInputPressed() const;
	bool IsDebugMenuTriggered() const;
	bool IsMeterTriggered() const;
};
