/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimHttpConnection.h"
#include "HALSimWeb.h"
#include "mimetypes.h"

#include <unistd.h>
#include <uv.h>

#include <wpi/FileSystem.h>
#include <wpi/SmallVector.h>
#include <wpi/UrlParser.h>
#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Request.h>

namespace uv = wpi::uv;

bool HALSimHttpConnection::IsValidWsUpgrade(wpi::StringRef protocol) {
  if (m_request.GetUrl() != "/ws") {
    SendError(404);
    return false;
  }

  return true;
}

void HALSimHttpConnection::ProcessWsUpgrade() {
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();  // one-shot

    if (!m_hws->RegisterWebsocket(shared_from_this())) {
      m_websocket->Fail(409, "Only a single simulation websocket is allowed");
      return;
    }
    m_isWsConnected = true;
    wpi::errs() << "websocket connected\n";
  });

  // parse incoming JSON, dispatch to parent
  m_websocket->text.connect([this](wpi::StringRef msg, bool) {
    if (!m_isWsConnected) {
      return;
    }
    wpi::json j;
    try {
      j = wpi::json::parse(msg);
    } catch (const wpi::json::parse_error& e) {
      std::string err("JSON parse failed: ");
      err += e.what();
      m_websocket->Fail(400, err);
      return;
    }
    m_hws->OnNetValueChanged(j);
  });

  m_websocket->closed([this](uint16_t, wpi::StringRef) {
    // unset the global, allow another websocket to connect
    m_isWsConnected = false;
  });
}

void HALSimHttpConnection::OnSimValueChanged(const wpi::json& msg) {}

class SendfileReq : public uv::RequestImpl<SendfileReq, uv_fs_t> {
 public:
  SendfileReq(uv_file out, uv_file in, int64_t inOffset, size_t len)
      : m_out(out), m_in(in), m_inOffset(inOffset), m_len(len) {
    error = [this](uv::Error err) { GetLoop().error(err); };
  }

  uv::Loop& GetLoop() const {
    return *static_cast<uv::Loop*>(GetRaw()->loop->data);
  }

  int Send(uv::Loop& loop) {
    int err = uv_fs_sendfile(loop.GetRaw(), GetRaw(), m_out, m_in, m_inOffset,
                             m_len, [](uv_fs_t* req) {
                               auto& h = *static_cast<SendfileReq*>(req->data);
                               if (req->result < 0) {
                                 h.ReportError(req->result);
                                 h.complete();
                                 h.Release();
                                 return;
                               }

                               h.m_inOffset += req->result;
                               h.m_len -= req->result;
                               if (h.m_len == 0) {
                                 // done
                                 h.complete();
                                 h.Release();  // this is always a one-shot
                                 return;
                               }

                               // need to send more
                               h.Send(h.GetLoop());
                             });
    if (err < 0) {
      ReportError(err);
      complete();
    }
    return err;
  }

  wpi::sig::Signal<> complete;

 private:
  uv_file m_out;
  uv_file m_in;
  int64_t m_inOffset;
  size_t m_len;
};

void Sendfile(uv::Loop& loop, uv_file out, uv_file in, int64_t inOffset,
              size_t len, std::function<void()> complete) {
  auto req = std::make_shared<SendfileReq>(out, in, inOffset, len);
  if (complete) req->complete.connect(complete);
  int err = req->Send(loop);
  if (err >= 0) req->Keep();
}

void HALSimHttpConnection::SendFileResponse(int code,
                                            const wpi::Twine& codeText,
                                            const wpi::Twine& contentType,
                                            const wpi::Twine& filename,
                                            const wpi::Twine& extraHeader) {
  // open file
  int infd;
  if (wpi::sys::fs::openFileForRead(filename, infd)) {
    SendError(404);
    return;
  }

  // get status (to get file size)
  wpi::sys::fs::file_status status;
  if (wpi::sys::fs::status(infd, status)) {
    SendError(404);
    ::close(infd);
    return;
  }

  uv_os_fd_t outfd;
  int err = uv_fileno(m_stream.GetRawHandle(), &outfd);
  if (err < 0) {
    m_stream.GetLoopRef().ReportError(err);
    SendError(404);
    ::close(infd);
    return;
  }

  wpi::SmallVector<uv::Buffer, 4> toSend;
  wpi::raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, status.getSize(), extraHeader);
  SendData(os.bufs(), false);

  // close after write completes if we aren't keeping alive
  // since we're using sendfile, set socket to blocking
  m_stream.SetBlocking(true);
  Sendfile(m_stream.GetLoopRef(), outfd, infd, 0, status.getSize(),
           [infd, closeAfter = !m_keepAlive, stream = &m_stream] {
             ::close(infd);
             if (closeAfter)
               stream->Close();
             else
               stream->SetBlocking(false);
           });
}

void HALSimHttpConnection::ProcessRequest() {
  // wpi::errs() << "HTTP request: '" << m_request.GetUrl() << "'\n";
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    SendError(400);
    return;
  }

  wpi::StringRef path;
  if (url.HasPath()) path = url.GetPath();
  // wpi::errs() << "path: \"" << path << "\"\n";

  wpi::StringRef query;
  if (url.HasQuery()) query = url.GetQuery();
  // wpi::errs() << "query: \"" << query << "\"\n";

  if (m_request.GetMethod() == wpi::HTTP_GET && path.startswith("/") &&
      !path.contains("..")) {
    if (path.endswith("/")) {
      path.
    }

    // convert to fs native representation

    // does it exist? ok, pass to underlying
    wpi::sys::fs::exists();

    auto contentType = MimeTypeFromPath(path);

    SendFileResponse(200, "OK", contentType, path);
  } else {
    SendError(404, "Resource not found");
  }
}