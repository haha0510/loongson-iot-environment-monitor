#ifndef NODE3_H
#define NODE3_H

#include <QWidget>
#include "widget.h"

QT_CHARTS_USE_NAMESPACE
namespace Ui {
class node3;
}

class node3 : public QWidget
{
    Q_OBJECT

public:
    explicit node3(QWidget *parent = nullptr);
    ~node3();

    void Init_Chart();
    void downline();
signals:
    void Signal_Return();//返回信号

    void Node3_Danger();
    void Node3_Safe();

private slots:

    void on_temperature3Button_clicked();

    void on_humi_3Button_clicked();

    void on_light_3Button_clicked();

    void on_smog_3Button_clicked();

    void on_pushButton_data_clicked();

    void on_back_3Button_clicked();

    void Slot_RefreshTime();//更新时间

    void Slot_Warning(unsigned int error_index);//警告和数据信息

private:
    Ui::node3 *ui;
};

#endif // NODE3_H
