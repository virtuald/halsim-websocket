/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Encoder.h"

#include <hal/Ports.h>
#include <mockdata/EncoderData.h>

void HALSimWSProviderEncoder::Initialize() {
  InitializeDefault(HAL_GetNumEncoders(), HALSIM_RegisterEncoderAllCallbacks);
}

wpi::json HALSimWSProviderEncoder::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init", (bool)HALSIM_GetEncoderInitialized(chan)},
      {">count", HALSIM_GetEncoderCount(chan)},
      {">period", HALSIM_GetEncoderPeriod(chan)},
      {"<reset", (bool)HALSIM_GetEncoderReset(chan)},
      {"<max_period", HALSIM_GetEncoderMaxPeriod(chan)},
      {"<reverse_direction", (bool)HALSIM_GetEncoderReverseDirection(chan)},
      {"<samples_to_avg", HALSIM_GetEncoderSamplesToAverage(chan)},
  };
}

void HALSimWSProviderEncoder::OnNetValueChanged(const CallbackInfo& info,
                                                const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">count")) != json.end()) {
    HALSIM_SetEncoderCount(info.channel, it.value());
  }
  if ((it = json.find(">period")) != json.end()) {
    HALSIM_SetEncoderPeriod(info.channel, it.value());
  }
}
