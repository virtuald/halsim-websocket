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

static HALSimWSProviderPWM pwm_provider;
static HALSimWSProviderDigitalPWM dpwm_provider;
static HALSimWSProviderDIO dio_provider;
static HALSimWSProviderAnalogIn ai_provider;
static HALSimWSProviderAnalogOut ao_provider;
static HALSimWSProviderDriverStation ds_provider;
static HALSimWSProviderEncoder encoder_provider;
static HALSimWSProviderRelay relay_provider;
static HALSimWSProviderRoboRIO roborio_provider;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "Websocket Simulator Initializing." << std::endl;
  auto hsw = std::make_shared<HALSimWeb>();
  if (!hsw->Initialize()) {
    return -1;
  }

  pwm_provider.Inject(hsw, "PWM");
  dpwm_provider.Inject(hsw, "dPWM");
  dio_provider.Inject(hsw, "DIO");
  ai_provider.Inject(hsw, "AI");
  ao_provider.Inject(hsw, "AO");
  ds_provider.Inject(hsw, "DriverStation");
  encoder_provider.Inject(hsw, "Encoder");
  relay_provider.Inject(hsw, "Relay");
  roborio_provider.Inject(hsw, "RoboRIO");

  auto hack = new std::shared_ptr<HALSimWeb>();
  hack->swap(hsw);

  HAL_SetMain(hack, HALSimWeb::Main, HALSimWeb::Exit);

  std::cout << "Websocket Simulator Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
