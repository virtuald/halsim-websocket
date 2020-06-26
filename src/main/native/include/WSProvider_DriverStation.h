/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "WSHalProviders.h"

class HALSimWSProviderDriverStation : public HALSimWSHalProvider {
 public:
  static void Initialize(std::weak_ptr<HALSimWeb> web,
                         WSRegisterFunc webRegisterFunc);

  using HALSimWSHalProvider::HALSimWSHalProvider;

  wpi::json OnSimValueChanged() override;
  void OnNetValueChanged(const wpi::json& json) override;
};
