#include "car_engine.h"

void CarEngine::Update() const
{
    // Implementation for updating the engine state
}

CarEngineData CarEngine::GetData() const
{
    

    return CarEngineData{
        fRPM_,
        fSpeed_,
        iGear_,
        fBoostPressure_,
        false
    };
}
