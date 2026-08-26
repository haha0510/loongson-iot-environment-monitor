#ifndef NODE1_H
#define NODE1_H

#include <QWidget>
#include "widget.h"


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class node1;
}

class node1 : public QWidget
{
    Q_OBJECT

public:
    explicit node1(QWidget *parent = nullptr);
    ~node1();

    void Init_Chart();
    void downline();
signals:
    void Signal_Return();//返回信号

    void Node1_Danger();
    void Node1_Safe();

private slots:


    void on_back_1Button_clicked();//返回
    void Slot_RefreshTime();//更新时间

    void Slot_Warning(unsigned int error_index);//警告和数据信息
    //四个曲线按钮、日志按钮槽函数自动生成

    void on_temperature1Button_clicked();

    void on_humi_1Button_clicked();

    void on_light_1Button_clicked();

    void on_smog_1Button_clicked();

    void on_pushButton_data_clicked();

private:
    Ui::node1 *ui;
};

#endif // NODE1_H
