#pragma once

#include "gauge_base.h"

// Tachometer + digital readout cluster. Display only: it receives CarEngineData
// from the engine each frame and renders the analog dial with an eased needle,
// plus the digital RPM / SPEED / GEAR readouts. It never computes engine values
// itself (rpm / speed are produced by CarEngine or the calling screen).
class CarMeter : public GaugeBase {
public:
	CarMeter();

	void Update(CarEngineData data) override;   // store data + ease the needle
	void Draw() override;                        // render dial, needle, readouts
	void PlayOpening() override;                 // startup needle sweep (stub)

	void Reset();                                // snap needle/values back to idle

private:
	float fNeedleRpm_;   // eased rpm actually shown by the needle
	float fRpm_;         // latest target rpm from engine data
	float fSpeed_;       // latest speed (km/h)
	int   iGear_;        // latest gear

	// point on the dial at the given radius for an rpm fraction (0..1)
	void DialPoint(float fRadius, float fFrac, int* iOutX, int* iOutY) const;
};
