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
#include <wpi/uv/Buffer.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

#include "WSBaseProvider.h"

class HALSimHttpConnection;

class HALSimWeb {
 public:
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;
  using ProviderList = wpi::StringMap<std::unique_ptr<HALSimWSBaseProvider>>;

  static std::shared_ptr<HALSimWeb> GetInstance() { return g_instance; }
  static void SetInstance(std::shared_ptr<HALSimWeb> inst) {
    g_instance = inst;
  }

  HALSimWeb(ProviderList& providers) : m_providers(providers) {}

  bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  bool RegisterWebsocket(std::shared_ptr<HALSimHttpConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimHttpConnection> hws);

  // sim -> network
  void SendUpdateToNet(const wpi::json& msg);

  // network -> sim
  void OnNetValueChanged(const wpi::json& msg);

  std::string UserWebRoot;
  std::string SystemWebRoot;

 private:
  static std::shared_ptr<HALSimWeb> g_instance;

  void MainLoop();

  // connected http connection that contains active websocket
  std::weak_ptr<HALSimHttpConnection> m_hws;

  // list of providers
  ProviderList& m_providers;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_server;

  // allows execution on main loop from threads
  std::weak_ptr<UvExecFunc> m_exec;

  // reusable buffers for sending data
  wpi::uv::SimpleBufferPool<4> m_buffers;
  wpi::mutex m_buffers_mutex;
};
