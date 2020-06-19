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

#include <wpi/json.h>
#include <wpi/mutex.h>

#include "WebSocketServerConnection.h"

class HALSimWeb;

class HALSimHttpConnection : public wpi::WebSocketServerConnection {
 public:
  explicit HALSimHttpConnection(HALSimWeb* hws,
                                std::shared_ptr<wpi::uv::Stream> stream)
      : wpi::WebSocketServerConnection(stream, {}), m_hws(hws) {}

 public:
  void OnSimValueChanged(const wpi::json& msg);

 protected:
  void ProcessRequest() override;
  virtual bool IsValidWsUpgrade(wpi::StringRef protocol) override;
  void ProcessWsUpgrade() override;
  void SendFileResponse(int code, const wpi::Twine& codeText,
                        const wpi::Twine& contentType,
                        const wpi::Twine& filename,
                        const wpi::Twine& extraHeader = wpi::Twine{});

  HALSimWeb* m_hws;
  bool m_isWsConnected = false;
};