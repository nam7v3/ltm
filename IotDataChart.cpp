#include "IotDataChart.h"
#include "ui_IotDataChart.h"
#include "server.h"

#include <QtLogging>
#include <QLineSeries>
#include <QValueAxis>
#include <QScatterSeries>

using Server = server::Server;
IotDataChart::IotDataChart(QWidget *parent) :
        QChartView(parent), ui(new Ui::IotDataChart), series(){
    ui->setupUi(this);

    connect(Server::instance(), &Server::newData, this, &IotDataChart::newData);
    connect(Server::instance(), &Server::nodeAdded, this, &IotDataChart::newNode);
    connect(Server::instance(), &Server::nodeRemoved, this, &IotDataChart::removeNode);

    auto xAxis = new QValueAxis();
    auto yAxis = new QValueAxis();

    xAxis->setRange(0, 256);
    xAxis->setTitleText("x");

    yAxis->setRange(0, 256);
    yAxis->setTitleText("y");

    chart()->addAxis(xAxis, Qt::AlignBottom);
    chart()->addAxis(yAxis, Qt::AlignLeft);
    setRenderHint(QPainter::Antialiasing);
}

IotDataChart::~IotDataChart() {
    delete ui;
}

void IotDataChart::newData(uint32_t id, uint32_t datax, uint32_t datay) {
    series[id]->append(datax, datay);

    update();
}

void IotDataChart::newNode(uint32_t id) {
    qInfo() << "IotDataChart::newNode: Id " << id;
    auto v = new QScatterSeries();
    series[id] = v;
    v->setName("Node #" + QString::number(id));
    v->setMarkerSize(10);
    chart()->addSeries(v);
    for(auto axis: chart()->axes()){
        v->attachAxis(axis);
    }
    update();
}

void IotDataChart::removeNode(uint32_t id) {
    auto s = series[id];
    series.remove(id);
    chart()->removeSeries(s);
    delete s;
}
