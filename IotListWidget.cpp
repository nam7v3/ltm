#include "IotListWidget.h"
#include "ui_IotListWidget.h"

#include <memory>
#include "server.h"

using namespace server;

IotListWidget::IotListWidget(QWidget *parent) :
        QListWidget(parent), ui(new Ui::IotListWidget) {
    ui->setupUi(this);
    connect(Server::instance(), &Server::nodeAdded, this, &IotListWidget::newConnection);
    connect(Server::instance(), &Server::nodeRemoved, this, &IotListWidget::removeConnection);
}

IotListWidget::~IotListWidget() {
    delete ui;
}

void IotListWidget::newConnection(uint32_t id) {
    auto item = new QListWidgetItem();
    addItem(item);
    item->setSizeHint(QSize(width(), 100));
    list[id] = item;
    setItemWidget(item, new IotItemWidget(id));
}

void IotListWidget::removeConnection(uint32_t id) {
    auto item = list[id];
    list.erase(id);
    delete takeItem(row(item));
}
