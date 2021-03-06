/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <functional>

#include <mockdata/NotifyListener.h>

#include <wpi/mutex.h>
#include <wpi/json.h>

#include "WSBaseProvider.h"

typedef void (*HALCbRegisterIndexedFunc)(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
typedef void (*HALCbRegisterSingleFunc)(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);

// provider generates diffs based on values
class HALSimWSHalProvider : public HALSimWSBaseProvider {
 public:
  using HALSimWSBaseProvider::HALSimWSBaseProvider;

  void OnNetworkConnected(std::shared_ptr<HALSimHttpConnection> ws);

  static void OnStaticSimCallback(const char* name, void* param,
                                  const struct HAL_Value* value);
  void OnSimCallback();

  // should return json structure representing sim value
  virtual wpi::json OnSimValueChanged() = 0;

 protected:
  // mutex protects last
  wpi::mutex mutex;
  wpi::json last;
};

// provider generates per-channel diffs
class HALSimWSHalChanProvider : public HALSimWSHalProvider {
 public:
  explicit HALSimWSHalChanProvider(int32_t channel, const std::string& key);

 protected:
  int32_t m_channel;
};

using WSRegisterFunc = std::function<void(
    const std::string&, std::shared_ptr<HALSimWSBaseProvider>)>;

template <typename T>
void CreateProviders(const std::string& prefix, int32_t numChannels,
                     HALCbRegisterIndexedFunc halRegisterFunc,
                     WSRegisterFunc webRegisterFunc);

template <typename T>
void CreateSingleProvider(const std::string& key,
                          HALCbRegisterSingleFunc halRegisterFunc,
                          WSRegisterFunc webRegisterFunc);

#include "WSHalProviders.inl"
