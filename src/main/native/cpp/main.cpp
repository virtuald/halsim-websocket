/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <hal/Main.h>

#include "HALSimWeb.h"
#include "WSProvider_Analog.h"
#include "WSProvider_DIO.h"
#include "WSProvider_DriverStation.h"
#include "WSProvider_Encoder.h"
#include "WSProvider_PWM.h"
#include "WSProvider_Relay.h"
#include "WSProvider_RoboRIO.h"
#include "WSProvider_dPWM.h"

// Currently, robots never terminate, so we keep static objects that are
// never properly released or cleaned up.
static wpi::StringMap<std::unique_ptr<HALSimWSBaseProvider>> providers;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "Websocket Simulator Initializing." << std::endl;
  auto hsw = std::make_shared<HALSimWeb>(providers);
  if (!hsw->Initialize()) {
    return -1;
  }

  auto registerFunc = [](const std::string& key,
                         std::unique_ptr<HALSimWSBaseProvider> provider) {
    providers[key] = std::move(provider);
  };

  HALSimWSProviderAnalogIn::Initialize(hsw, registerFunc);
  HALSimWSProviderAnalogOut::Initialize(hsw, registerFunc);
  HALSimWSProviderDIO::Initialize(hsw, registerFunc);
  HALSimWSProviderDigitalPWM::Initialize(hsw, registerFunc);
  HALSimWSProviderDriverStation::Initialize(hsw, registerFunc);
  HALSimWSProviderEncoder::Initialize(hsw, registerFunc);
  HALSimWSProviderPWM::Initialize(hsw, registerFunc);
  HALSimWSProviderRelay::Initialize(hsw, registerFunc);
  HALSimWSProviderRoboRIO::Initialize(hsw, registerFunc);

  auto hack = new std::shared_ptr<HALSimWeb>(hsw);
  HAL_SetMain(hack, HALSimWeb::Main, HALSimWeb::Exit);

  std::cout << "Websocket Simulator Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
