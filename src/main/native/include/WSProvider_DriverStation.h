/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "WSProvider.h"

class HALSimWSProviderDriverStation : public HALSimWSProvider {
 public:
  void Initialize() override;
  wpi::json OnSimValueChanged(uint32_t chan) override;
  void OnNetValueChanged(const CallbackInfo& info,
                         const wpi::json& json) override;
};
