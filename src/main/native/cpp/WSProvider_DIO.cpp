/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_DIO.h"

#include <hal/Ports.h>
#include <mockdata/DIOData.h>

void HALSimWSProviderDIO::Initialize() {
  InitializeDefault(HAL_GetNumDigitalChannels(),
                    HALSIM_RegisterDIOAllCallbacks);
}

wpi::json HALSimWSProviderDIO::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init", (bool)HALSIM_GetDIOInitialized(chan)},
      {"<>value", (bool)HALSIM_GetDIOValue(chan)},
      {"<pulse_length", HALSIM_GetDIOPulseLength(chan)},
      {"<input", (bool)HALSIM_GetDIOIsInput(chan)},
  };
}

void HALSimWSProviderDIO::OnNetValueChanged(const CallbackInfo& info,
                                            const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find("<>value")) != json.end()) {
    HALSIM_SetDIOValue(info.channel, (bool)it.value());
  }
}
