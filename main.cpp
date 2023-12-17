#include <QApplication>

#include "mainwindow.h"

namespace net = boost::asio;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    server::Server::instance()->run(server::tcp::endpoint{net::ip::make_address("0.0.0.0"), 8080});

    MainWindow w;
    w.show();
    QObject::connect(&a, &QApplication::aboutToQuit, Server::instance(), &Server::quit);
    return a.exec();
}
