/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_WEBSOCKETSERVERCONNECTION_H_
#define WPIUTIL_WPI_WEBSOCKETSERVERCONNECTION_H_

#include <initializer_list>
#include <memory>
#include <string>

#include "wpi/ArrayRef.h"
#include "wpi/HttpServerConnection.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/WebSocket.h"
#include "wpi/WebSocketServer.h"

namespace wpi {

namespace uv {
class Stream;
}  // namespace uv

/**
 * A server-side HTTP connection that also accepts WebSocket upgrades.
 */
class WebSocketServerConnection
    : public HttpServerConnection,
      public std::enable_shared_from_this<WebSocketServerConnection> {
 public:
  /**
   * Constructor.
   *
   * @param stream network stream
   * @param protocols Acceptable subprotocols
   */
  explicit WebSocketServerConnection(std::shared_ptr<uv::Stream> stream,
                                     ArrayRef<StringRef> protocols);

  /**
   * Constructor.
   *
   * @param stream network stream
   * @param protocols Acceptable subprotocols
   */
  explicit WebSocketServerConnection(std::shared_ptr<uv::Stream> stream,
                                     std::initializer_list<StringRef> protocols)
      : WebSocketServerConnection(
            stream, makeArrayRef(protocols.begin(), protocols.end())) {}

 protected:
  /**
   * Check that an incoming WebSocket upgrade is okay.  This is called prior
   * to accepting the upgrade (so prior to ProcessWsUpgrade()).
   *
   * The implementation should check other headers and return true if the
   * WebSocket connection should be accepted.
   *
   * @param protocol negotiated subprotocol
   */
  virtual bool IsValidWsUpgrade(StringRef protocol) { return true; }

  /**
   * Process an incoming WebSocket upgrade.  This is called after the header
   * reader has been disconnected and the websocket has been accepted.
   *
   * The implementation should set up appropriate callbacks on the websocket
   * object to continue communication.
   *
   * @note When a WebSocket upgrade occurs, the stream user data is replaced
   *       with the websocket, and the websocket user data points to "this".
   *       Replace the websocket user data with caution!
   */
  virtual void ProcessWsUpgrade() = 0;

  /**
   * WebSocket connection; not valid until ProcessWsUpgrade is called.
   */
  WebSocket* m_websocket = nullptr;

 private:
  WebSocketServerHelper m_helper;
  SmallVector<std::string, 2> m_protocols;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_WEBSOCKETSERVERCONNECTION_H_