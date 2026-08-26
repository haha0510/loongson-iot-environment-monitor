#ifndef NODE2_H
#define NODE2_H

#include <QWidget>
#include "widget.h"


QT_CHARTS_USE_NAMESPACE
namespace Ui {
class node2;
}

class node2 : public QWidget
{
    Q_OBJECT

public:
    explicit node2(QWidget *parent = nullptr);
    ~node2();

    void Init_Chart();
    void downline();
signals:
    void Signal_Return();//返回信号

    void Node2_Danger();
    void Node2_Safe();
private slots:
    void on_temperature2Button_clicked();

    void on_humi_2Button_clicked();

    void on_light_2Button_clicked();

    void on_smog_2Button_clicked();

    void on_pushButton_data_clicked();

    void Slot_RefreshTime();//更新时间

    void Slot_Warning(unsigned int error_index);//警告和数据信息
    //四个曲线按钮、日志按钮槽函数自动生成

    void on_back_2Button_clicked();

private:
    Ui::node2 *ui;
};

#endif // NODE2_H
