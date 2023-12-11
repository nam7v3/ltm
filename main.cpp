#include <QApplication>

#include "mainwindow.h"

namespace net = boost::asio;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
