#include "../header/share.h"
#include "../header/typedef.h"
#include "../DxLib/DxLib.h"

Color::Color(uchar r, uchar g, uchar b)
	: r_(r), g_(g), b_(b) {}

int Color::Code() const
{
	return GetColor(r_, g_, b_);
}

const Color Color::RED    (255,   0,   0);
const Color Color::GREEN  (  0, 255,   0);
const Color Color::BLUE   (  0,   0, 255);
const Color Color::YELLOW (255, 255,   0);
const Color Color::MAGENTA(255,   0, 255);
const Color Color::CYAN   (  0, 255, 255);
const Color Color::WHITE  (255, 255, 255);
const Color Color::BLACK  (  0,   0,   0);
