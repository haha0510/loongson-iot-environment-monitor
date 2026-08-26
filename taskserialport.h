#ifndef TASKSERIALPORT_H
#define TASKSERIALPORT_H

#include <QObject>

#include "widget.h"

class taskserialport : public QObject
{
    Q_OBJECT
public:
    explicit taskserialport(QObject *parent = nullptr);

    QSerialPort *serialPort;//声明串口信息类
    void Init_SerialPort();
    bool sendATCommand(const std::string& command,const std::string& expected,int timeout);
signals:
     void Signal_SerialPortReadBuffer();//串口读取信号

public slots:
    void Slot_SerialPortReadBuffer();//返回串口接受信息槽函数

    // void Slot_Node1_SerialDanger();//数据异常信号的槽函数，向连接的串口设备发送警报
    // void Slot_Node1_SerialSafe();
    // void Slot_Node2_SerialDanger();
    // void Slot_Node2_SerialSafe();
    // void Slot_Node3_SerialDanger();
    // void Slot_Node3_SerialSafe();
};

#endif // TASKSERIALPORT_H
