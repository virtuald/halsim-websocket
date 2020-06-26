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

void HALSimWSProviderAnalogIn::Initialize(std::weak_ptr<HALSimWeb> web,
                                          WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogIn>("AI", HAL_GetNumAnalogInputs(),
                                            HALSIM_RegisterAnalogInAllCallbacks,
                                            web, webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogIn::OnSimValueChanged() {
  return {
      {"<init", (bool)HALSIM_GetAnalogInInitialized(m_channel)},
      {"<avg_bits", HALSIM_GetAnalogInAverageBits(m_channel)},
      {"<oversample_bits", HALSIM_GetAnalogInOversampleBits(m_channel)},
      {">voltage", HALSIM_GetAnalogInVoltage(m_channel)},
      {
          "accum",
          {
              {"<init",
               (bool)HALSIM_GetAnalogInAccumulatorInitialized(m_channel)},
              {">value", HALSIM_GetAnalogInAccumulatorValue(m_channel)},
              {">count", HALSIM_GetAnalogInAccumulatorCount(m_channel)},
              {"<center", HALSIM_GetAnalogInAccumulatorCenter(m_channel)},
              {"<deadband", HALSIM_GetAnalogInAccumulatorDeadband(m_channel)},
          },
      },
  };
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">voltage")) != json.end()) {
    HALSIM_SetAnalogInVoltage(m_channel, it.value());
  }
  if ((it = json.find("accum")) != json.end()) {
    auto accum = it.value();
    if ((it = accum.find(">value")) != accum.end()) {
      HALSIM_SetAnalogInAccumulatorValue(m_channel, it.value());
    }
    if ((it = json.find(">count")) != json.end()) {
      HALSIM_SetAnalogInAccumulatorCount(m_channel, it.value());
    }
  }
}

void HALSimWSProviderAnalogOut::Initialize(std::weak_ptr<HALSimWeb> web,
                                           WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogOut>(
      "AO", HAL_GetNumAnalogOutputs(), HALSIM_RegisterAnalogOutAllCallbacks,
      web, webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogOut::OnSimValueChanged() {
  return {{"<init", HALSIM_GetAnalogOutInitialized(m_channel)},
          {"<voltage", HALSIM_GetAnalogOutVoltage(m_channel)}};
}
