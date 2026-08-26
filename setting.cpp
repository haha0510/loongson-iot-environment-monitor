#include "setting.h"
#include "ui_setting.h"

setting::setting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::setting)
{
    ui->setupUi(this);

    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0xdceaf6));

    window()->setPalette(pal);
    pal.setColor(QPalette::WindowText, Qt::blue);
    ui->label_time->setPalette(pal);
    QFont ft;
    ft.setPointSize(15);
    ui->label_time->setFont(ft);

    temp_max = ui->doubleSpinBox_tempMax->value();
    temp_min = ui->doubleSpinBox_tempMin->value();
    humi_max = ui->doubleSpinBox_humiMax->value();
    humi_min = ui->doubleSpinBox_humiMin->value();
    light_max = ui->doubleSpinBox_lightMax->value();
    light_min = ui->doubleSpinBox_lightMin->value();
    smog_max = ui->doubleSpinBox_smogMax->value();


}

setting::~setting()
{
    delete ui;
}

void setting::on_backButton_clicked()
{
     emit Signal_Return();
}

void setting::Slot_RefreshTime()
{
    ui->label_time->setText(stringCurrentTime);

}

void setting::on_doubleSpinBox_tempMax_valueChanged(double arg1)
{
    if(arg1 < temp_min)
    {
        QMessageBox::warning(this, "Warning", "温度上限值不能小于下限值!请重新设置！");
    }
    else
    {
        temp_max = arg1;
    }
}


void setting::on_doubleSpinBox_tempMin_valueChanged(double arg1)
{
    if(arg1 > temp_max)
    {
        QMessageBox::warning(this, "Warning", "温度下限值不能大于上限值!请重新设置！");
    }
    else
    {
        temp_min = arg1;
    }
}


void setting::on_doubleSpinBox_humiMax_valueChanged(double arg1)
{
    if(arg1 < humi_min)
    {
        QMessageBox::warning(this, "Warning", "湿度上限值不能小于下限值!请重新设置!");
    }
    else
    {
        humi_max = arg1;
    }
}


void setting::on_doubleSpinBox_humiMin_valueChanged(double arg1)
{
    if(arg1 > humi_max)
    {
        QMessageBox::warning(this, "Warning", "湿度上限值不能小于下限值!请重新设置!");
    }
    else
    {
        humi_min = arg1;
    }
}


void setting::on_doubleSpinBox_lightMax_valueChanged(double arg1)
{
    if(arg1 < light_min)
    {
        QMessageBox::warning(this, "Warning", "光照上限值不能小于下限值!请重新设置!");
    }
    else
    {
        light_max = arg1;
    }
}


void setting::on_doubleSpinBox_lightMin_valueChanged(double arg1)
{
    if(arg1 > light_max)
    {
        QMessageBox::warning(this, "Warning", "光照下限值不能大于上限值!请重新设置!");
    }
    else
    {
        light_min = arg1;
    }
}

