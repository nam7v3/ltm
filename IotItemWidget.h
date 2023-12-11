#ifndef LTM_APP_IOTITEMWIDGET_H
#define LTM_APP_IOTITEMWIDGET_H

#include <QWidget>
#include <QStateMachine>


QT_BEGIN_NAMESPACE
namespace Ui { class IotItemWidget; }
QT_END_NAMESPACE

class IotItemWidget : public QWidget {
Q_OBJECT

public:
    explicit IotItemWidget(uint32_t id, QWidget *parent = nullptr);

    ~IotItemWidget() override;

    uint32_t getId() const;

private:
    Ui::IotItemWidget *ui;
    uint32_t id;
    QStateMachine pauseStateMachine;
};


#endif //LTM_APP_IOTITEMWIDGET_H
