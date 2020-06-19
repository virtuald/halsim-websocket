/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PWM.h"

#include <hal/Ports.h>
#include <mockdata/PWMData.h>

void HALSimWSProviderPWM::Initialize() {
  InitializeDefault(HAL_GetNumPWMChannels(), HALSIM_RegisterPWMAllCallbacks);
}

wpi::json HALSimWSProviderPWM::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init", (bool)HALSIM_GetPWMInitialized(chan)},
      {"<speed", HALSIM_GetPWMSpeed(chan)},
      {"<position", HALSIM_GetPWMPosition(chan)},
      {"<raw", HALSIM_GetPWMRawValue(chan)},
      {"<period_scale", HALSIM_GetPWMPeriodScale(chan)},
      {"<zero_latch", (bool)HALSIM_GetPWMZeroLatch(chan)},
  };
}
