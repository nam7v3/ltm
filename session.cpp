#include "session.h"

#include "server.h"

#include <QDebug>

#include <boost/beast/core/bind_handler.hpp>

namespace server {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

Session::Session(uint32_t id, tcp::socket &&socket)
    : mId(id), mWs(std::move(socket)){}

Session::~Session(){
}

void Session::run() {
    Server::instance()->addSession(mId, shared_from_this());

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

void Session::pause(){
    isPause = true;
    doWrite(IOT_MESSAGE_PAUSE);
}

void Session::unpause(){
    isPause = false;
    doWrite(IOT_MESSAGE_UNPAUSE);
}

void Session::togglePause() {
    if(!isPause) pause();
    else unpause();
}

void Session::quit(){
    std::string message(IOT_MESSAGE_QUIT);
    mWs.async_write(
            boost::asio::buffer(message.data(), message.size()),
            beast::bind_front_handler(&Session::onQuit, shared_from_this()));
}

void Session::onQuit(boost::beast::error_code ec, std::size_t nbytes){
    if(ec){
        qFatal() << "Session::onQuit: " << ec.what();
    }
    Server::instance()->removeSession(mId);
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
        qFatal() << "Session::onWrite: " << ec.what();
    }
}

void Session::doRead() {
    mWs.binary(true);
    mWs.async_read(
        mBuffer, beast::bind_front_handler(&Session::onRead, shared_from_this()));
}

void Session::onRead(beast::error_code ec, std::size_t nbytes) {
    if (ec) {
        return;
    }
    if (nbytes == sizeof(uint32_t) * 2) {
        uint32_t recvInt[2];
        boost::asio::buffer_copy(boost::asio::buffer(&recvInt, sizeof(recvInt)), mBuffer.data());
        emit Server::instance()->newData(mId, recvInt[0], recvInt[1]);
    }
    mBuffer.consume(mBuffer.size());
    doRead();
}


} // namespace server
