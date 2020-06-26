/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWeb.h"
#include "HALSimHttpConnection.h"

#include <wpi/raw_uv_ostream.h>
#include <wpi/Twine.h>
#include <wpi/UrlParser.h>
#include <wpi/WebSocketServer.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

namespace uv = wpi::uv;

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

  // TODO: configurable port
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

bool HALSimWeb::RegisterWebsocket(std::shared_ptr<HALSimHttpConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }
  m_hws = hws;
  return true;
}

void HALSimWeb::CloseWebsocket(std::shared_ptr<HALSimHttpConnection> hws) {
  if (hws == m_hws.lock()) {
    m_hws.reset();
  }
}

void HALSimWeb::SendUpdateToNet(const wpi::json& msg) {
  // note: function called from arbitrary threads

  if (auto exec = m_exec.lock()) {
    // render json to buffers
    wpi::SmallVector<uv::Buffer, 4> sendBufs;
    wpi::raw_uv_ostream os{sendBufs, [this]() -> uv::Buffer {
                             std::lock_guard lock(m_buffers_mutex);
                             return m_buffers.Allocate();
                           }};
    os << msg;

    // call the websocket send function on the uv loop
    exec->Call([this, sendBufs]() mutable {
      if (auto hws = m_hws.lock()) {
        hws->OnSimValueChanged(sendBufs, [this](auto bufs) {
          std::lock_guard lock(m_buffers_mutex);
          m_buffers.Release(bufs);
        });
      } else {
        std::lock_guard lock(m_buffers_mutex);
        m_buffers.Release(sendBufs);
      }
    });
  }
}

void HALSimWeb::OnNetValueChanged(const wpi::json& msg) {
  // TODO: error handling for bad keys
  for (auto iter = msg.cbegin(); iter != msg.cend(); ++iter) {
    auto fiter = m_providers.find(iter.key());
    if (fiter != m_providers.end()) {
      fiter->second->OnNetValueChanged(iter.value());
    }
  }
}
