/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSHalProviders.h"

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

void HALSimWSHalProvider::OnSimCallback(const char* name, void* param,
                                        const struct HAL_Value* value) {
  auto provider = static_cast<HALSimWSHalProvider*>(param);

  // perform diff
  wpi::json diff;
  {
    std::lock_guard lock(provider->mutex);
    auto result = provider->OnSimValueChanged();
    performDiff(provider->last, result, diff);
    provider->last = std::move(result);
  }

  // send it out if it's not empty
  if (!diff.empty()) {
    provider->SendUpdateToNet(diff);
  }
}

HALSimWSHalChanProvider::HALSimWSHalChanProvider(int32_t channel,
                                                 const std::string& key)
    : HALSimWSHalProvider(key), m_channel(channel) {}
