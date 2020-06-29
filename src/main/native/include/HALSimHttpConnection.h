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
#include <wpi/uv/Buffer.h>

#include "WebSocketServerConnection.h"

class HALSimWeb;

class HALSimHttpConnection : public wpi::WebSocketServerConnection {
 public:
  using ReleaseFunc =
      std::function<void(wpi::MutableArrayRef<wpi::uv::Buffer>)>;

  explicit HALSimHttpConnection(std::shared_ptr<wpi::uv::Stream> stream)
      : wpi::WebSocketServerConnection(stream, {}) {}

 public:
  void OnSimValueChanged(wpi::ArrayRef<wpi::uv::Buffer> data,
                         ReleaseFunc release);

 protected:
  void ProcessRequest() override;
  virtual bool IsValidWsUpgrade(wpi::StringRef protocol) override;
  void ProcessWsUpgrade() override;
  void SendFileResponse(int code, const wpi::Twine& codeText,
                        const wpi::Twine& contentType,
                        const wpi::Twine& filename,
                        const wpi::Twine& extraHeader = wpi::Twine{});

  bool m_isWsConnected = false;
};