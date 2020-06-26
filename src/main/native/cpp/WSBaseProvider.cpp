/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSBaseProvider.h"
#include "HALSimWeb.h"

HALSimWSBaseProvider::HALSimWSBaseProvider(const std::string& key,
                                           std::weak_ptr<HALSimWeb> web)
    : m_key(key), m_web(web) {}

void HALSimWSBaseProvider::SendUpdateToNet(const wpi::json& update) {
  auto web = m_web.lock();
  if (web) {
    wpi::json netValue = {
        {m_key, update},
    };
    web->SendUpdateToNet(netValue);
  }
}

void HALSimWSBaseProvider::OnNetValueChanged(const wpi::json& json) {
  // empty
}
