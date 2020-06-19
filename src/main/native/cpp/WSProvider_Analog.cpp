/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Analog.h"

#include <hal/Ports.h>
#include <mockdata/AnalogInData.h>
#include <mockdata/AnalogOutData.h>

void HALSimWSProviderAnalogIn::Initialize() {
  InitializeDefault(HAL_GetNumAnalogInputs(),
                    HALSIM_RegisterAnalogInAllCallbacks);
}

wpi::json HALSimWSProviderAnalogIn::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init", (bool)HALSIM_GetAnalogInInitialized(chan)},
      {"<avg_bits", HALSIM_GetAnalogInAverageBits(chan)},
      {"<oversample_bits", HALSIM_GetAnalogInOversampleBits(chan)},
      {">voltage", HALSIM_GetAnalogInVoltage(chan)},
      {
          "accum",
          {
              {"<init", (bool)HALSIM_GetAnalogInAccumulatorInitialized(chan)},
              {">value", HALSIM_GetAnalogInAccumulatorValue(chan)},
              {">count", HALSIM_GetAnalogInAccumulatorCount(chan)},
              {"<center", HALSIM_GetAnalogInAccumulatorCenter(chan)},
              {"<deadband", HALSIM_GetAnalogInAccumulatorDeadband(chan)},
          },
      },
  };
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const CallbackInfo& info,
                                                 const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">voltage")) != json.end()) {
    HALSIM_SetAnalogInVoltage(info.channel, it.value());
  }
  if ((it = json.find("accum")) != json.end()) {
    auto accum = it.value();
    if ((it = accum.find(">value")) != accum.end()) {
      HALSIM_SetAnalogInAccumulatorValue(info.channel, it.value());
    }
    if ((it = json.find(">count")) != json.end()) {
      HALSIM_SetAnalogInAccumulatorCount(info.channel, it.value());
    }
  }
}

void HALSimWSProviderAnalogOut::Initialize() {
  InitializeDefault(HAL_GetNumAnalogOutputs(),
                    HALSIM_RegisterAnalogOutAllCallbacks);
}

wpi::json HALSimWSProviderAnalogOut::OnSimValueChanged(uint32_t chan) {
  return {{"<init", HALSIM_GetAnalogOutInitialized(chan)},
          {"<voltage", HALSIM_GetAnalogOutVoltage(chan)}};
}
