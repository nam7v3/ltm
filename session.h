#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <deque>

namespace server {

using tcp = boost::asio::ip::tcp;

class Server;

#define IOT_MESSAGE_PAUSE "PAUSE"
#define IOT_MESSAGE_UNPAUSE "UNPAUSE"

class Session : public std::enable_shared_from_this<Session> {
 public:
  explicit Session(Server& server, uint32_t id, tcp::socket&& socket);

  void run();
  void showData();
  void pause();
  void unpause();

 private:
  void onAccept(boost::beast::error_code ec);

  void doWrite(std::string message);
  void onWrite(boost::beast::error_code ec, std::size_t nbytes);


  void doRead();
  void onRead(boost::beast::error_code ec, std::size_t nbytes);

 private:
  uint32_t mId;
  boost::beast::websocket::stream<boost::beast::tcp_stream> mWs;
  boost::beast::flat_buffer mBuffer;
  std::deque<uint32_t> mData;
  Server& mServer;
};

}  // namespace server
#endif
