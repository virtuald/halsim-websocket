/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_RoboRIO.h"

#include <hal/Ports.h>
#include <mockdata/RoboRioData.h>

void HALSimWSProviderRoboRIO::Initialize() {
  InitializeDefault(1, HALSIM_RegisterRoboRioAllCallbacks);
}

wpi::json HALSimWSProviderRoboRIO::OnSimValueChanged(uint32_t chan) {
  return {
      {">fpga_button", (bool)HALSIM_GetRoboRioFPGAButton(chan)},

      {">vin_voltage", HALSIM_GetRoboRioVInVoltage(chan)},
      {">vin_current", HALSIM_GetRoboRioVInCurrent(chan)},

      {">6v_voltage", HALSIM_GetRoboRioUserVoltage6V(chan)},
      {">6v_current", HALSIM_GetRoboRioUserCurrent6V(chan)},
      {">6v_active", (bool)HALSIM_GetRoboRioUserActive6V(chan)},
      {">6v_faults", HALSIM_GetRoboRioUserFaults6V(chan)},

      {">5v_voltage", HALSIM_GetRoboRioUserVoltage5V(chan)},
      {">5v_current", HALSIM_GetRoboRioUserCurrent5V(chan)},
      {">5v_active", (bool)HALSIM_GetRoboRioUserActive5V(chan)},
      {">5v_faults", HALSIM_GetRoboRioUserFaults5V(chan)},

      {">3v3_voltage", HALSIM_GetRoboRioUserVoltage3V3(chan)},
      {">3v3_current", HALSIM_GetRoboRioUserCurrent3V3(chan)},
      {">3v3_active", (bool)HALSIM_GetRoboRioUserActive3V3(chan)},
      {">3v3_faults", HALSIM_GetRoboRioUserFaults3V3(chan)},
  };
}

void HALSimWSProviderRoboRIO::OnNetValueChanged(const CallbackInfo& info,
                                                const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">fpga_button")) != json.end()) {
    HALSIM_SetRoboRioFPGAButton(info.channel, (bool)it.value());
  }

  if ((it = json.find(">vin_voltage")) != json.end()) {
    HALSIM_SetRoboRioVInVoltage(info.channel, it.value());
  }
  if ((it = json.find(">vin_current")) != json.end()) {
    HALSIM_SetRoboRioVInCurrent(info.channel, it.value());
  }

  if ((it = json.find(">6v_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage6V(info.channel, it.value());
  }
  if ((it = json.find(">6v_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent6V(info.channel, it.value());
  }
  if ((it = json.find(">6v_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive6V(info.channel, (bool)it.value());
  }
  if ((it = json.find(">6v_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults6V(info.channel, it.value());
  }

  if ((it = json.find(">5v_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage5V(info.channel, it.value());
  }
  if ((it = json.find(">5v_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent5V(info.channel, it.value());
  }
  if ((it = json.find(">5v_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive5V(info.channel, (bool)it.value());
  }
  if ((it = json.find(">5v_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults5V(info.channel, it.value());
  }

  if ((it = json.find(">3v3_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage3V3(info.channel, it.value());
  }
  if ((it = json.find(">3v3_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent3V3(info.channel, it.value());
  }
  if ((it = json.find(">3v3_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive3V3(info.channel, (bool)it.value());
  }
  if ((it = json.find(">3v3_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults3V3(info.channel, it.value());
  }
}
