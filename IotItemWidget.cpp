#include "IotItemWidget.h"
#include "ui_IotItemWidget.h"

#include <QState>
#include "server.h"

using Server = server::Server;

IotItemWidget::IotItemWidget(uint32_t id, QWidget *parent) :
        QWidget(parent), ui(new Ui::IotItemWidget), id(id), pauseStateMachine(new QStateMachine){
    ui->setupUi(this);
    ui->nodeLabel->setText(QString("Node #") + QString::number(id));

    QState *pauseState = new QState();
    pauseState->assignProperty(ui->pauseButton, "text", "Pause");
    pauseState->setObjectName("pause");

    QState *unpauseState = new QState();
    unpauseState->setObjectName("unpause");
    unpauseState->assignProperty(ui->pauseButton, "text", "Unpause");

    pauseState->addTransition(ui->pauseButton, &QAbstractButton::clicked, unpauseState);
    unpauseState->addTransition(ui->pauseButton, &QAbstractButton::clicked, pauseState);

    pauseStateMachine.addState(pauseState);
    pauseStateMachine.addState(unpauseState);

    pauseStateMachine.setInitialState(pauseState);
    pauseStateMachine.start();

    connect(ui->quitButton, &QPushButton::pressed, [id](){
        Server::instance()->quitNode(id);
    });
    connect(ui->pauseButton, &QPushButton::pressed, [id](){
        Server::instance()->togglePauseNode(id);
    });
}

IotItemWidget::~IotItemWidget() {
    delete ui;
}

uint32_t IotItemWidget::getId() const{
    return id;
}
