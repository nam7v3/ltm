#include "session.h"

#include "server.h"
#include "util.h"

#include <boost/beast/core/bind_handler.hpp>
#include <iostream>

namespace server {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

Session::Session(Server &server, uint32_t id, tcp::socket &&socket)
    : mId(id), mWs(std::move(socket)), mServer(server) {}

void Session::run() {
  mServer.addNode(mId, shared_from_this());

  mWs.set_option(
      websocket::stream_base::timeout::suggested(beast::role_type::server));
  mWs.set_option(
      websocket::stream_base::decorator([](websocket::response_type &res) {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                         " websocket-server-async");
      }));

  mWs.async_accept(
      beast::bind_front_handler(&Session::onAccept, shared_from_this()));
}

void Session::showData() {
  for (auto &data : mData) {
    std::cout << data << " ";
  }
}

void Session::pause(){
  doWrite(IOT_MESSAGE_PAUSE);
}

void Session::unpause(){
  doWrite(IOT_MESSAGE_UNPAUSE);
}

void Session::onAccept(beast::error_code ec) {
  if (ec)
    return;
  doRead();
}

void Session::doWrite(std::string message) {
  mWs.async_write(
      boost::asio::buffer(message.data(), message.size()),
      beast::bind_front_handler(&Session::onWrite, shared_from_this()));
}

void Session::onWrite(boost::beast::error_code ec, std::size_t nbytes) {
  if(ec){
    fail(ec, "async_write");
  }
}

void Session::doRead() {
  mWs.binary(true);
  mWs.async_read(
      mBuffer, beast::bind_front_handler(&Session::onRead, shared_from_this()));
}

void Session::onRead(beast::error_code ec, std::size_t nbytes) {
  if (ec) {
    mServer.removeNode(mId);
    return;
  }
  if (nbytes == sizeof(int)) {
    uint32_t recvInt;
    std::memcpy(&recvInt, mBuffer.data().data(), sizeof(uint32_t));

    mData.push_back(recvInt);
    std::cout << mId << ": " << recvInt << std::endl;
  }
  mBuffer.consume(mBuffer.size());
  doRead();
}
} // namespace server
