#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>


#include <cstdint>

#include <deque>

namespace server {

using tcp = boost::asio::ip::tcp;

class Server;

#define IOT_MESSAGE_PAUSE "PAUSE"
#define IOT_MESSAGE_UNPAUSE "UNPAUSE"
#define IOT_MESSAGE_QUIT "QUIT"

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(uint32_t id, tcp::socket&& socket);
    ~Session();
    void run();
    void pause();
    void unpause();
    void togglePause();
    void quit();

private:
    void onAccept(boost::beast::error_code ec);

    void doWrite(std::string message);
    void onWrite(boost::beast::error_code ec, std::size_t nbytes);

    void doRead();
    void onRead(boost::beast::error_code ec, std::size_t nbytes);

    void onQuit(boost::beast::error_code ec, std::size_t nbytes);

private:
    uint32_t mId;
    boost::beast::websocket::stream<boost::beast::tcp_stream> mWs;
    boost::beast::flat_buffer mBuffer;
    bool isPause = false;
};

}  // namespace server

#endif // SESSION_H
