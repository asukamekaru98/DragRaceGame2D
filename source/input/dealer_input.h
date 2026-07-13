#pragma once

#include "key_input.h"

// Key input for the car dealer screen
class DealerInput : public KeyInput {
private:
	bool bIsLeftTriggered_;
	bool bIsRightTriggered_;
	bool bIsDecideTriggered_;
	bool bIsCancelTriggered_;

public:
	DealerInput();
	void Update() override;
	bool IsLeftTriggered() const;
	bool IsRightTriggered() const;
	bool IsDecideTriggered() const;
	bool IsCancelTriggered() const;
};
