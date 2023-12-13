#ifndef LTM_APP_IOTDATACHART_H
#define LTM_APP_IOTDATACHART_H

#include <QChartView>
#include <QModelIndex>
#include <QListWidgetItem>
#include <QChart>

#include <map>
#include <QLineSeries>
#include <QScatterSeries>


QT_BEGIN_NAMESPACE
namespace Ui { class IotDataChart; }
QT_END_NAMESPACE

class IotDataChart : public QChartView {
Q_OBJECT

public:
    explicit IotDataChart(QWidget *parent = nullptr);

    ~IotDataChart() override;
public slots:
    void newData(uint32_t id, uint32_t datax, uint32_t datay);
    void newNode(uint32_t id);
    void removeNode(uint32_t id);
private:
    Ui::IotDataChart *ui;
    QMap<uint32_t, QScatterSeries *> series;
};


#endif //LTM_APP_IOTDATACHART_H
