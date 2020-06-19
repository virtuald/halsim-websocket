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
#include <wpi/mutex.h>
#include <wpi/StringMap.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

class HALSimHttpConnection;
class HALSimWSProvider::CallbackInfo;

class HALSimWeb {
 public:
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = uv::AsyncFunction<void(LoopFunc)>;

  bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  void RegisterProvider(HALSimWSProvider::CallbackInfo* cb);

  bool RegisterWebsocket(std::shared_ptr<HALSimHttpConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimHttpConnection> hws);

  // sim -> network
  void OnSimValueChanged(const wpi::json& msg);

  // network -> sim
  void OnNetValueChanged(const wpi::json& msg);

  std::string UserWebRoot;
  std::string SystemWebRoot;

 private:
  void MainLoop();

  // connected http connection that contains active websocket
  std::weak_ptr<HALSimHttpConnection> m_hws;

  // list of providers
  wpi::StringMap<HALSimWSProvider::CallbackInfo*> m_providers;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_server;

  std::weak_ptr<UvExecFunc> m_exec;
};
