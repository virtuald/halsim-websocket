/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/json.h>

class HALSimWeb;

class HALSimWSBaseProvider {
 public:
  explicit HALSimWSBaseProvider(const std::string& key,
                                std::weak_ptr<HALSimWeb> web);
  virtual ~HALSimWSBaseProvider() {}

  // network -> sim
  virtual void OnNetValueChanged(const wpi::json& json);

 protected:
  // sim -> network
  void SendUpdateToNet(const wpi::json& update);

 private:
  std::string m_key;
  std::weak_ptr<HALSimWeb> m_web;
};
