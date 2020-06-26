/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Relay.h"

#include <hal/Ports.h>
#include <mockdata/RelayData.h>

void HALSimWSProviderRelay::Initialize(std::weak_ptr<HALSimWeb> web,
                                       WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderRelay>("Relay", HAL_GetNumRelayHeaders(),
                                         HALSIM_RegisterRelayAllCallbacks, web,
                                         webRegisterFunc);
}

wpi::json HALSimWSProviderRelay::OnSimValueChanged() {
  return {
      {"<init_fwd", (bool)HALSIM_GetRelayInitializedForward(m_channel)},
      {"<init_rev", (bool)HALSIM_GetRelayInitializedReverse(m_channel)},
      {"<fwd", (bool)HALSIM_GetRelayForward(m_channel)},
      {"<rev", (bool)HALSIM_GetRelayReverse(m_channel)},
  };
}
