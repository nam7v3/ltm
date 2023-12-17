#include "server.h"

#include "session.h"

#include <QtLogging>
#include <QDebug>

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <memory>
#include <mutex>
#include <utility>


namespace beast = boost::beast;
namespace net = boost::asio;
namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;

namespace server {

net::io_context ioc{7};

Server* Server::_instance = nullptr;

Server *Server::instance() {
    if(!Server::_instance) {
        Server::_instance = new Server();
    }
    return Server::_instance;
}

Server::Server() : mIdGen{0}, mIoc(ioc), mAcceptor(ioc), mWork(net::make_work_guard(ioc)){}

void Server::run(const tcp::endpoint& endpoint) {
    beast::error_code ec;

    mAcceptor.open(endpoint.protocol(), ec);
    if (ec) {
        qFatal() << "Server::run::open " << ec.what();
        return;
    }

    mAcceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        qFatal() << "Server::run::set_option " << ec.what();
        return;
    }

    mAcceptor.bind(endpoint, ec);
    if (ec) {
        qFatal() << "Server::run::bind " << ec.what();
        return;
    }

    mAcceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        qFatal() << "Server::run::listen " << ec.what();
        return;
    }
    qDebug() << "Server: Accepting connection";
    doAccept();

    mThreads.reserve(7);
    for (auto i = 7; i > 0; --i)
        mThreads.emplace_back([] { ioc.run(); });
}

void Server::quit() {
    quitAllNode();
    mWork.reset();
    mIoc.stop();
}

void Server::doAccept() {
    mAcceptor.async_accept(
        net::make_strand(mIoc),
        beast::bind_front_handler(&Server::onAccept, this));
}

void Server::onAccept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        qFatal() << "Server::onAccept" << ec.what();
    } else {
        std::make_shared<Session>(mIdGen, std::move(socket))->run();
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

void Server::togglePauseNode(uint32_t id) {
    std::lock_guard<std::mutex> lock(mMutex);
    mSessions[id]->togglePause();
}


void Server::quitNode(uint32_t id) {
    std::lock_guard<std::mutex> lock(mMutex);
    mSessions[id]->quit();
}

void Server::quitAllNode(){
    std::lock_guard<std::mutex> lock(mMutex);
    for(auto [k, v]: mSessions){
        v->quit();
    }
}

void Server::addSession(uint32_t id, std::shared_ptr<Session> session) {
    mMutex.lock();
        mSessions[id] = std::move(session);
        mIdGen++;
    mMutex.unlock();

    emit nodeAdded(id);
}

void Server::removeSession(uint32_t id) {
    mMutex.lock();
        mSessions.erase(id);
    mMutex.unlock();

    emit nodeRemoved(id);
}
} // namespace server
