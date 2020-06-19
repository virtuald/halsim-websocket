/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Relay.h"

#include <hal/Ports.h>
#include <mockdata/RelayData.h>

void HALSimWSProviderRelay::Initialize() {
  InitializeDefault(HAL_GetNumRelayHeaders(), HALSIM_RegisterRelayAllCallbacks);
}

wpi::json HALSimWSProviderRelay::OnSimValueChanged(uint32_t chan) {
  return {
      {"<init_fwd", (bool)HALSIM_GetRelayInitializedForward(chan)},
      {"<init_rev", (bool)HALSIM_GetRelayInitializedReverse(chan)},
      {"<fwd", (bool)HALSIM_GetRelayForward(chan)},
      {"<rev", (bool)HALSIM_GetRelayReverse(chan)},
  };
}
