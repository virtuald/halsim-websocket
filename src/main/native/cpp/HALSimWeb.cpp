/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWeb.h"
#include "HALSimHttpConnection.h"
#include "WSProvider.h"

#include <wpi/raw_ostream.h>
#include <wpi/Twine.h>
#include <wpi/UrlParser.h>
#include <wpi/WebSocketServer.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

namespace uv = wpi::uv;

// TODO: not thread safe?
static SimpleBufferPool<4>& GetBufferPool() {
  static SimpleBufferPool<4> bufferPool;
  return bufferPool;
}

bool HALSimWeb::Initialize() {
  // XXX: determine user/system web folders to retrieve static
  //      content from

  m_loop = uv::Loop::Create();
  if (!m_loop) {
    return false;
  }

  m_loop->error.connect(
      [](uv::Error err) { wpi::errs() << "uv ERROR: " << err.str() << '\n'; });

  m_server = uv::Tcp::Create(m_loop);
  if (!m_server) {
    return false;
  }

  m_server->Bind("", 8080);
  return true;
}

void HALSimWeb::Main(void* param) {
  auto hack = (std::shared_ptr<HALSimWeb>*)param;
  (*hack)->MainLoop();
  delete hack;
}

void HALSimWeb::MainLoop() {
  m_exec = UvExecFunc::Create(m_loop, [](auto out, LoopFunc func) {
    func();
    out.set_value();
  });

  // when we get a connection, accept it and start reading
  m_server->connection.connect([this, srv = m_server.get()] {
    auto tcp = srv->Accept();
    if (!tcp) return;
    wpi::errs() << "Got a connection\n";
    auto conn = std::make_shared<HALSimHttpConnection>(this, tcp);
    tcp->SetData(conn);
  });

  // start listening for incoming connections
  m_server->Listen();
  wpi::errs() << "Listening on port 8080\n";

  m_loop->Run();
}

void HALSimWeb::Exit(void* param) {
  auto hack = (std::shared_ptr<HALSimWeb>*)param;
  auto loop = (*hack)->m_loop;
  loop->Walk([](uv::Handle& h) {
    h.SetLoopClosing(true);
    h.Close();
  });
}

void HALSimWeb::RegisterProvider(HALSimWSProvider::CallbackInfo* cb) {
  m_providers[cb->key] = cb;
}

void HALSimWeb::OnSimValueChanged(const wpi::json& msg) {
  if (auto exec = m_exec.lock()) {
    // ... uh, maybe move the msg into the lambda
    exec->Call([this]() {
      if (auto hws = m_hws.lock()) {
        // TODO: serialize to buffer, buffer needs to hang out
        // until message is sent; can't pass the JSON through
        // here unfortunately
        hws->OnSimValueChanged(msg);

        // they usually need a callback to Dealloc() each buffer,
        // whatever that seems to mean
      }
    });
  }
}

void HALSimWeb::OnNetValueChanged(const wpi::json& msg) {
  // TODO: error handling for bad keys
  for (auto iter = msg.cbegin(); iter != msg.cend(); ++iter) {
    if (auto info = m_providers.lookup(iter.key())) {
      info->provider->OnNetValueChanged(*info, iter.value());
    }
  }
}
