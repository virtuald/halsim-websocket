/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

#include <mockdata/NotifyListener.h>

#include <wpi/json.h>

#include "HALSimWeb.h"

typedef void (*HALCbRegisterIndexedFunc)(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
typedef void (*HALCbRegisterSingleFunc)(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);

void WSProviderOnSimCallback(const char* name, void* param,
                             const struct HAL_Value* value);

class HALSimWSProvider {
 public:
  struct CallbackInfo {
    HALSimWSProvider* provider;
    std::string key;
    int channel;

    // mutex protects last
    wpi::mutex mutex;
    wpi::json last;
  };

  void Inject(std::shared_ptr<HALSimWeb> parent, std::string keyPrefix);

  // Initialize is called by inject.
  virtual void Initialize() = 0;
  virtual void InitializeDefault(int numChannels,
                                 HALCbRegisterIndexedFunc registerFunc);
  virtual void InitializeDefaultSingle(HALCbRegisterSingleFunc registerFunc);

  // sim -> network
  virtual wpi::json OnSimValueChanged(uint32_t channel) = 0;

  // network -> sim
  virtual void OnNetValueChanged(const CallbackInfo& info,
                                 const wpi::json& json){};

  int numChannels;
  std::string keyPrefix;

  std::shared_ptr<HALSimWeb> parent;
  std::vector<std::shared_ptr<CallbackInfo>> cbInfos;
};
