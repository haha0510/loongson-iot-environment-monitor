#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include "taskserialport.h"
#include "widget.h"

namespace Ui {
class setting;
}

class setting : public QWidget
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = nullptr);
    ~setting();

signals:
    void Signal_Return();

private slots:
    void on_backButton_clicked();

    void Slot_RefreshTime();//更新时间槽函数

    void on_doubleSpinBox_tempMax_valueChanged(double arg1);

    void on_doubleSpinBox_tempMin_valueChanged(double arg1);

    void on_doubleSpinBox_humiMax_valueChanged(double arg1);

    void on_doubleSpinBox_humiMin_valueChanged(double arg1);

    void on_doubleSpinBox_lightMax_valueChanged(double arg1);

    void on_doubleSpinBox_lightMin_valueChanged(double arg1);

private:
    Ui::setting *ui;
};

#endif // SETTING_H
