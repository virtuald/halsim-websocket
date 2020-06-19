/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider.h"

#include <wpi/raw_ostream.h>
#include <wpi/Twine.h>
#include <wpi/UrlParser.h>
#include <wpi/WebSocketServer.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

namespace uv = wpi::uv;

void HALSimWSProvider::Inject(std::shared_ptr<HALSimWeb> parentArg,
                              std::string keyPrefix) {
  parent = parentArg;
  this->keyPrefix = std::move(keyPrefix);

  this->Initialize();
}

static void performDiff(const wpi::json& last, const wpi::json& result,
                        wpi::json& out) {
  for (auto iter = result.cbegin(); iter != result.cend(); ++iter) {
    auto key = iter.key();
    auto value = iter.value();
    auto otherIter = last.find(key);
    if (otherIter == last.end()) {
      out[key] = value;
    } else {
      auto otherValue = otherIter.value();

      // object comparisons should recurse
      if (value.type() == wpi::json::value_t::object) {
        wpi::json vout;
        performDiff(otherValue, value, vout);
        if (!vout.empty()) {
          out[key] = vout;
        }

        // everything else is just a comparison
        // .. this would be inefficient with an array of objects, so don't do
        // that!
      } else if (otherValue != value) {
        out[key] = value;
      }
    }
  }
}

void WSProviderOnSimCallback(const char* name, void* param,
                             const struct HAL_Value* value) {
  auto info = static_cast<struct HALSimWSProvider::CallbackInfo*>(param);
  uint32_t chan = static_cast<uint32_t>(info->channel);
  auto provider = info->provider;
  auto result = provider->OnSimValueChanged(chan);

  // perform diff
  wpi::json diff;
  {
    std::lock_guard lock(info->mutex);
    performDiff(info->last, result, diff);
    info->last = std::move(result);
  }

  // send it out if it's not empty
  if (!diff.empty()) {
    wpi::json retval = {
        {info->key, diff},
    };
    provider->parent->OnSimValueChanged(retval);
  }
}

void HALSimWSProvider::InitializeDefault(
    int numChannels, HALCbRegisterIndexedFunc registerFunc) {
  this->numChannels = numChannels;
  cbInfos.reserve(numChannels);
  for (int i = 0; i < numChannels; i++) {
    std::shared_ptr<CallbackInfo> info(
        new CallbackInfo{this, (keyPrefix + "/" + wpi::Twine(i)).str(), i, {}});
    cbInfos.emplace_back(info);
  }

  for (auto info : cbInfos) {
    registerFunc(info->channel, WSProviderOnSimCallback, info.get(), true);
    parent->RegisterProvider(info.get());
  }
}

void HALSimWSProvider::InitializeDefaultSingle(
    HALCbRegisterSingleFunc registerFunc) {
  std::shared_ptr<CallbackInfo> info(new CallbackInfo{this, keyPrefix, 0, {}});
  cbInfos.emplace_back(info);

  for (auto info : cbInfos) {
    registerFunc(WSProviderOnSimCallback, info.get(), true);
  }
}
