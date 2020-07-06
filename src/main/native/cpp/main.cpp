/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <hal/Main.h>

#include "HALSimWeb.h"
#include "ProviderContainer.h"
#include "WSProvider_Analog.h"
#include "WSProvider_DIO.h"
#include "WSProvider_DriverStation.h"
#include "WSProvider_Encoder.h"
#include "WSProvider_PWM.h"
#include "WSProvider_Relay.h"
#include "WSProvider_RoboRIO.h"
#include "WSProvider_dPWM.h"
#include "WSProvider_SimDevice.h"

using namespace std::placeholders;

// Currently, robots never terminate, so we keep static objects that are
// never properly released or cleaned up.
static ProviderContainer providers;
static HALSimWSProviderSimDevices simDevices(providers);

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "Websocket Simulator Initializing." << std::endl;
  auto hsw = std::make_shared<HALSimWeb>(providers);
  HALSimWeb::SetInstance(hsw);

  if (!hsw->Initialize()) {
    return -1;
  }

  WSRegisterFunc registerFunc =
      std::bind(&ProviderContainer::Add, &providers, _1, _2);

  HALSimWSProviderAnalogIn::Initialize(registerFunc);
  HALSimWSProviderAnalogOut::Initialize(registerFunc);
  HALSimWSProviderDIO::Initialize(registerFunc);
  HALSimWSProviderDigitalPWM::Initialize(registerFunc);
  HALSimWSProviderDriverStation::Initialize(registerFunc);
  HALSimWSProviderEncoder::Initialize(registerFunc);
  HALSimWSProviderPWM::Initialize(registerFunc);
  HALSimWSProviderRelay::Initialize(registerFunc);
  HALSimWSProviderRoboRIO::Initialize(registerFunc);

  simDevices.Initialize();

  HAL_SetMain(nullptr, HALSimWeb::Main, HALSimWeb::Exit);

  std::cout << "Websocket Simulator Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
