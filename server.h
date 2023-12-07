#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/websocket.hpp>
#include <map>
#include <memory>

namespace server {

class Session;

using tcp = boost::asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server> {
public:
  Server(boost::asio::io_context &ioc);
  void run(tcp::endpoint endpoint);
  void pauseNode(uint32_t id);
  void unpauseNode(uint32_t id);
  void addNode(uint32_t id, std::shared_ptr<Session> session);
  void removeNode(uint32_t id);

private:
  void doAccept();
  void onAccept(boost::beast::error_code ec, tcp::socket socket);

private:
  uint32_t mIdGen;
  boost::asio::io_context &mIoc;
  tcp::acceptor mAcceptor;
  std::map<uint32_t, std::shared_ptr<Session>> mSessions;
  std::mutex mMutex;
};
} // namespace server

#endif
