/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_dPWM.h"

#include <hal/Ports.h>
#include <mockdata/DigitalPWMData.h>

void HALSimWSProviderDigitalPWM::Initialize() {
  InitializeDefault(HAL_GetNumDigitalPWMOutputs(),
                    HALSIM_RegisterDigitalPWMAllCallbacks);
}

wpi::json HALSimWSProviderDigitalPWM::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init", (bool)HALSIM_GetDigitalPWMInitialized(chan)},
      {"<dio_pin", HALSIM_GetDigitalPWMPin(chan)},
      {"<duty_cycle", HALSIM_GetDigitalPWMDutyCycle(chan)},
  };
}
