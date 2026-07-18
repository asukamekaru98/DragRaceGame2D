#include "../../DxLib/DxLib.h"
#include <math.h>
#include "car_meter.h"
#include "../share.h"

// ---- gauge calibration ----
#define MTR_RPM_IDLE     800.0f
#define MTR_RPM_REDLINE  6800.0f
#define MTR_RPM_MAX      8000.0f
#define MTR_SMOOTH       0.15f     // needle easing (smaller = smoother / slower)
#define MTR_TACH_START   210.0f    // needle angle (deg) at rpm 0
#define MTR_TACH_SWEEP   240.0f    // total sweep (deg)

// ---- dial layout ----
#define DIAL_CX          260
#define DIAL_CY          330
#define DIAL_R_OUTER     180.0f
#define DIAL_R_TICK      160.0f
#define DIAL_R_NEEDLE    150.0f

// ---- digital readout panel ----
#define PANEL_X          520
#define PANEL_Y          110
#define PANEL_DY         50

CarMeter::CarMeter()
	: fNeedleRpm_(MTR_RPM_IDLE), fRpm_(MTR_RPM_IDLE), fSpeed_(0.0f), iGear_(1)
{
}

void CarMeter::Reset()
{
	fNeedleRpm_ = MTR_RPM_IDLE;
	fRpm_       = MTR_RPM_IDLE;
	fSpeed_     = 0.0f;
	iGear_      = 1;
}

void CarMeter::Update(CarEngineData data)
{
	fRpm_   = data.fRpm;
	fSpeed_ = data.fSpeed;
	iGear_  = data.iGear;

	// needle easing -- lags the target rpm for a smooth sweep
	fNeedleRpm_ += (fRpm_ - fNeedleRpm_) * MTR_SMOOTH;
}

void CarMeter::DialPoint(float fRadius, float fFrac, int* iOutX, int* iOutY) const
{
	float fDeg = MTR_TACH_START - fFrac * MTR_TACH_SWEEP;
	float fRad = fDeg * 3.14159265f / 180.0f;
	*iOutX = DIAL_CX + (int)(cosf(fRad) * fRadius);
	*iOutY = DIAL_CY - (int)(sinf(fRad) * fRadius);   // minus: screen y grows downward
}

void CarMeter::Draw()
{
	// ---- tachometer dial ----
	DrawCircle(DIAL_CX, DIAL_CY, (int)DIAL_R_OUTER + 6, Color(45, 45, 55).Code(), TRUE);
	DrawCircle(DIAL_CX, DIAL_CY, (int)DIAL_R_OUTER,     Color(15, 15, 20).Code(), TRUE);

	// redline band (short radial segments)
	for (float f = MTR_RPM_REDLINE / MTR_RPM_MAX; f <= 1.0f; f += 0.008f) {
		int ix, iy, ox, oy;
		DialPoint(DIAL_R_TICK - 14, f, &ix, &iy);
		DialPoint(DIAL_R_OUTER - 2, f, &ox, &oy);
		DrawLine(ix, iy, ox, oy, Color::RED.Code());
	}

	// ticks + labels (0..8 = x1000 r/min)
	for (int i = 0; i <= 8; i++) {
		float f = (float)i / 8.0f;
		int ix, iy, ox, oy, lx, ly;
		DialPoint(DIAL_R_TICK - 18, f, &ix, &iy);
		DialPoint(DIAL_R_TICK,      f, &ox, &oy);
		DrawLine(ix, iy, ox, oy, Color::WHITE.Code());
		DialPoint(DIAL_R_TICK - 40, f, &lx, &ly);
		DrawFormatString(lx - 6, ly - 8, Color(200, 200, 200).Code(), "%d", i);
	}

	// needle (uses the eased value, not the raw target)
	float fFrac = fNeedleRpm_ / MTR_RPM_MAX;
	if (fFrac > 1.0f) fFrac = 1.0f;
	int nx, ny;
	DialPoint(DIAL_R_NEEDLE, fFrac, &nx, &ny);
	// thick needle: triangle from a small base around the hub
	float dx = (float)(nx - DIAL_CX), dy = (float)(ny - DIAL_CY);
	float len = sqrtf(dx * dx + dy * dy);
	if (len < 1.0f) len = 1.0f;
	float px = -dy / len * 7.0f, py = dx / len * 7.0f;   // perpendicular * half width
	int needleCol = (fNeedleRpm_ >= MTR_RPM_REDLINE) ? Color::RED.Code()
	                                                 : Color(255, 80, 60).Code();
	DrawTriangle(nx, ny,
	             DIAL_CX + (int)px, DIAL_CY + (int)py,
	             DIAL_CX - (int)px, DIAL_CY - (int)py, needleCol, TRUE);
	DrawCircle(DIAL_CX, DIAL_CY, 18, Color(70, 70, 80).Code(), TRUE);
	DrawCircle(DIAL_CX, DIAL_CY, 8,  Color(180, 180, 190).Code(), TRUE);
	DrawString(DIAL_CX - 40, DIAL_CY + 70, "x1000 r/min", Color(120, 120, 130).Code());

	// ---- digital readouts ----
	DrawString(PANEL_X, PANEL_Y, "RPM", Color(150, 150, 160).Code());
	DrawFormatString(PANEL_X + 90, PANEL_Y, Color::WHITE.Code(), "%5d", (int)fNeedleRpm_);

	DrawString(PANEL_X, PANEL_Y + PANEL_DY, "SPEED", Color(150, 150, 160).Code());
	DrawFormatString(PANEL_X + 90, PANEL_Y + PANEL_DY, Color(120, 255, 120).Code(),
	                 "%6.1f km/h", fSpeed_);

	DrawString(PANEL_X, PANEL_Y + PANEL_DY * 2, "GEAR", Color(150, 150, 160).Code());
	DrawFormatString(PANEL_X + 90, PANEL_Y + PANEL_DY * 2, Color::YELLOW.Code(), "%d", iGear_);
}

void CarMeter::PlayOpening()
{
	// TODO: startup needle sweep (0 -> MAX -> idle). Stub for now.
}
