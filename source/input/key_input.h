#pragma once

#define KEY_INPUT_KEY_COUNT 256

class KeyInput {	// abstract
public:
	KeyInput();
	virtual ~KeyInput();
	virtual void Update() = 0;

private:
	char cPrevKeyState_[KEY_INPUT_KEY_COUNT];
	char cCurrKeyState_[KEY_INPUT_KEY_COUNT];

protected:
	void RefreshKeyState();
	bool IsPressed(const int iKeyCode) const;
	bool IsTriggered(const int iKeyCode) const;
};

// マージテスト用のコメント1

// マージテスト用のコメント2