#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/websocket.hpp>

#include <QObject>

#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <list>

namespace server {

extern boost::asio::io_context ioc;

class Session;

using tcp = boost::asio::ip::tcp;

class Server : public QObject, public std::enable_shared_from_this<Server> {
    Q_OBJECT
public:
    static Server *instance();

    void run(const tcp::endpoint& endpoint);

    void addSession(uint32_t id, std::shared_ptr<Session> session);
    void removeSession(uint32_t id);

protected:
    explicit Server();


public slots:
    void quitNode(uint32_t id);
    void pauseNode(uint32_t id);
    void unpauseNode(uint32_t id);
    void togglePauseNode(uint32_t id);

    void quitAllNode();
    void quit();

signals:
    void newData(uint32_t id, uint32_t datax, uint32_t datay);
    void nodeAdded(uint32_t id);
    void nodeRemoved(uint32_t id);

private:
    void doAccept();
    void onAccept(boost::beast::error_code ec, tcp::socket socket);

private:
    static Server *_instance;

private:
    uint32_t mIdGen;
    boost::asio::io_context &mIoc;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type > mWork;
    tcp::acceptor mAcceptor;
    std::vector<std::thread> mThreads;
    std::map<uint32_t, std::shared_ptr<Session>> mSessions;
    std::mutex mMutex;
};
} // namespace server


#endif // SERVER_H
