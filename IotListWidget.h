#ifndef LTM_APP_IOTLISTWIDGET_H
#define LTM_APP_IOTLISTWIDGET_H

#include <QListWidget>
#include <utility>
#include "IotItemWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class IotListWidget; }
QT_END_NAMESPACE

class IotListWidget : public QListWidget {
Q_OBJECT

public:
    explicit IotListWidget(QWidget *parent = nullptr);

    ~IotListWidget() override;
private slots:
    void newConnection(uint32_t id);
    void removeConnection(uint32_t id);
private:
    Ui::IotListWidget *ui;
    std::map<uint32_t, QListWidgetItem *> list;
};


#endif //LTM_APP_IOTLISTWIDGET_H
