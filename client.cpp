#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/websocket.hpp>

#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/system/detail/error_code.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class IotNode {
public:
  explicit IotNode(asio::io_context &ioc, int sec)
      : mSec(sec), mTimer(ioc), mWs(ioc), mResolver(ioc) {
    srand(time(NULL));
  }

  void run(const std::string &host, const std::string &port) {
    auto const results = mResolver.resolve(host, port);
    auto ep = net::connect(mWs.next_layer(), results);

    std::string path = host + ':' + std::to_string(ep.port());
    mWs.set_option(
        websocket::stream_base::decorator([](websocket::request_type &req) {
          req.set(http::field::user_agent,
                  std::string(BOOST_BEAST_VERSION_STRING) +
                      " websocket-client");
        }));

    mWs.handshake(host, "/");
    doRead();
    doWrite();
  }

private:
  void generateData() {
    mData[0] = rand() % 256;
    mData[1] = rand() % 256;
  }

  void doRead() {
    mWs.async_read(mReadBuffer, [this](const boost::system::error_code &ec,
                                       std::size_t size) {
      if (ec) {
        return;
      }
      if(!mRunning){
        return;
      }
      std::cout << "Recv: " << beast::make_printable(mReadBuffer.data())
                << std::endl;
      std::string recvMsg = beast::buffers_to_string(mReadBuffer.data());
      if (recvMsg == "PAUSE") {
        mPause = true;
        std::cout << "Pausing node..." << std::endl;
      } else if (recvMsg == "UNPAUSE") {
        mPause = false;
        std::cout << "Unpausing node..." << std::endl;
        doWrite();
      } else if (recvMsg == "QUIT") {
        std::cout << "Quitting node..." << std::endl;
        doClose();
      }
      mReadBuffer.clear();
      doRead();
    });
  }

  void doWrite() {
    mWs.binary(true);
    mTimer.async_wait([this](const boost::system::error_code &ec) {
      if (!ec && !mPause && mRunning) {
        generateData();
        mWs.async_write(
            asio::buffer(&mData, sizeof(mData)),
            [this](const boost::system::error_code &ec, std::size_t) {
              if (!ec) {
                mTimer.expires_from_now(std::chrono::seconds(mSec));
                doWrite();
              } else {
                std::cerr << "doWrite: " <<ec.what() << std::endl;
              }
            });
      }
    });
  }

  void doClose() {
    mWs.async_close(websocket::close_code::normal,
                    [this](const boost::system::error_code &ec) {
                      if (ec) { }
                      mRunning = false;
                    });
  }

private:
  uint32_t mData[2];
  bool mPause = false;
  bool mRunning = true;;
  int mSec;
  beast::flat_buffer mReadBuffer;
  asio::steady_timer mTimer;
  websocket::stream<tcp::socket> mWs;
  tcp::resolver mResolver;
};

int main(int argc, char **argv) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: " << argv[0] << " <host> <port> <sec>\n"
                << "Example:\n"
                << "    " << argv[0] << " localhost 8080 3\n";
      return EXIT_FAILURE;
    }
    net::io_context ioc;

    int nsec = atoi(argv[3]);
    IotNode node(ioc, nsec);
    node.run(argv[1], argv[2]);

    ioc.run();

  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
