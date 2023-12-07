#include "server.h"

#include "session.h"
#include "util.h"

#include <algorithm>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/error.hpp>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


namespace server {

Server::Server(net::io_context &ioc) : mIdGen{0}, mIoc(ioc), mAcceptor(ioc) {}

void Server::run(tcp::endpoint endpoint) {
  beast::error_code ec;

  mAcceptor.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }

  mAcceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  mAcceptor.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  mAcceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }
  doAccept();
}

void Server::doAccept() {
  mAcceptor.async_accept(
      net::make_strand(mIoc),
      beast::bind_front_handler(&Server::onAccept, shared_from_this()));
}

void Server::onAccept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    fail(ec, "accept");
  } else {
    std::make_shared<Session>(*this, mIdGen, std::move(socket))->run();
  }
  doAccept();
}

void Server::pauseNode(uint32_t id) {
  std::lock_guard<std::mutex> lock(mMutex);
  mSessions[id]->pause();
}

void Server::unpauseNode(uint32_t id) {
  std::lock_guard<std::mutex> lock(mMutex);
  mSessions[id]->unpause();
}

void Server::addNode(uint32_t id, std::shared_ptr<Session> session) {
  std::lock_guard<std::mutex> lock(mMutex);
  mSessions[id] = session;
  mIdGen++;
}

void Server::removeNode(uint32_t id) {
  std::lock_guard<std::mutex> lock(mMutex);
  mSessions.erase(id);
}
} // namespace server
