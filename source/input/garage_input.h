#pragma once

#include "key_input.h"

// Key input for the garage screen
class GarageInput : public KeyInput {
private:
	bool bIsMenuUpTriggered_;
	bool bIsMenuDownTriggered_;
	bool bIsCarPrevTriggered_;
	bool bIsCarNextTriggered_;
	bool bIsDecideTriggered_;

public:
	GarageInput();
	void Update() override;
	bool IsMenuUpTriggered() const;
	bool IsMenuDownTriggered() const;
	bool IsCarPrevTriggered() const;
	bool IsCarNextTriggered() const;
	bool IsDecideTriggered() const;
};
