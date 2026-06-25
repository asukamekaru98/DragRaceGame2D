#pragma once

// 色を表す値オブジェクト。DxLib の GetColor を隠蔽し、
// 描画系には Code() が返すカラーコードのみを渡す。
class Color {
private:
	uchar r_, g_, b_;

public:
	// constructor
	Color(uchar r, uchar g, uchar b);

	int Code() const;

	// 純色の名前付き定数を共有メンバとして宣言
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
	static const Color MAGENTA;
	static const Color CYAN;
	static const Color WHITE;
	static const Color BLACK;
};
