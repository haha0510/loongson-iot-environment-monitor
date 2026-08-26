#include "taskserialport.h"
#include <QDebug>
#include <QElapsedTimer>

taskserialport::taskserialport(QObject *parent)
    : QObject{parent}
{

}

bool taskserialport::sendATCommand(const std::string& command,const std::string& expected,int timeout){


    serialPort->clear(QSerialPort::Input);

    QByteArray data(command.data(), command.size());
    serialPort->write(data);

    QByteArray response;
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeout) {
        if (serialPort->waitForReadyRead(50)) { // 每100ms检查一次
            response +=  serialPort->readAll();

            // 检查是否收到完整响应
            if (response.contains(expected.c_str())) {
                qDebug() << "GOT EXPECTED WORD!";
                qDebug() << "FULL RESPONSE:" << response;
                return true;
            }

            // 检查错误响应
            if (response.contains("ERROR") || response.contains("FAIL")) {
                qDebug() << "ERROR RESPONSE:" << response;
                return false;
            }
        }
    }
    qDebug() << "TIMEOUT, PARTIAL RESPONSE:" << response;
    return false;
}



void taskserialport::Init_SerialPort()
{

    /* 搜索所有可有串口 */
    QList<QSerialPortInfo> serialPortInfo = QSerialPortInfo::availablePorts();//定义一次QSerialPortInfo类的列表,名字为serialPortInfo
    if(serialPortInfo.isEmpty())//如果列表为空
    {
        // 显示警告信息或者处理没有串行端口的情况
         QMessageBox::information(nullptr, "Warning", "No serial ports available!!!");
        return;
    }
    // qDebug() << "Available Ports:";//打印所有串口可用的信息
    // foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
    //     qDebug() << "Port:" << info.portName()
    //     << "Description:" << info.description()
    //     << "Manufacturer:" << info.manufacturer();
    // }
    // 手动选择包含"USB"或"CH340"的串口（ESP8266常用芯片）
    QString targetPort;
    foreach(const QSerialPortInfo &info, serialPortInfo){
        if(info.description().contains("USB")){
            targetPort = info.portName();
            break;
        }
    }
     serialPort = new QSerialPort;

    if(!targetPort.isEmpty()){
        serialPort->setPortName(targetPort);
         qDebug() << "setPortName success!";
    }
    //serialPort->setPort(serialPortInfo[0]); // 使用安全的索引，绑定到第一个检测到的串口，setPort指定要操作的串口设备


    serialPort->setBaudRate(QSerialPort::Baud115200);//设置波特率115200
    serialPort->setDataBits(QSerialPort::Data8);//设置数据位8位
    serialPort->setParity(QSerialPort::NoParity);//校验位无
    serialPort->setStopBits(QSerialPort::OneStop);//停止位1位
    serialPort->setFlowControl(QSerialPort::NoFlowControl);//流控无


    if (serialPort->open(QIODevice::ReadWrite) != true) {   //判断串口是否打开成功
        //QMessageBox::information(nullptr, "Warning", "Failed to open serial port!");
        qDebug() << "Failed to open serial port!";
        return;
    }

    // serialPort->write("AT\r\n");
    // serialPort->write("AT+CWMODE=2\r\n");
    // serialPort->write("AT+RST\r\n");
    // serialPort->write("AT+CWSAP=\"esp8266\",\"wenjie01\",5,3\r\n");
    // serialPort->write("AT+CIPMUX=1\r\n");
    // serialPort->write("AT+CIPSERVER=1,8080\r\n");

    if (!sendATCommand("AT\r\n","OK",60)) {
        qDebug() << "No Response to:" << "AT\r\n";
        return ;
    }
    if (!sendATCommand("AT+CWMODE=2\r\n","OK",60)){
        qDebug() << "No Response to:" << "AT+CWMODE=2\r\n";
        return ;
    }
    if (!sendATCommand("AT+RST\r\n","ready",500)){
        qDebug() << "No Response to:" << "AT+RST\r\n";
        return ;
    }

    if (!sendATCommand("AT+CWSAP=\"esp8266\",\"wenjie01\",5,3\r\n","OK",100)){
        qDebug() << "No Response to:" << "AT+CWSAP=\"esp8266\",\"wenjie01\",5,3\r\n";
        return ;
    }

    if (!sendATCommand("AT+CIPMUX=1\r\n","OK",100)) {
        qDebug() << "No Response to:" << "AT+CIPMUX=1\r\n";
        return ;
    }
    if (!sendATCommand("AT+CIPSERVER=1,8080\r\n","OK",100)) {
        qDebug() << "No Response to:" << "AT+CIPSERVER=1,8080\r\n";
        return ;
    }



    //将串口读取到的信息，通过Slot_SerialPortReadBuffer()槽函数返回widget
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(Slot_SerialPortReadBuffer()));

}

void taskserialport::Slot_SerialPortReadBuffer()
{
    serialPortReadBuffer.append(serialPort->readAll());

    emit Signal_SerialPortReadBuffer();//发送串口读取信息信号
}
// void taskserialport::Slot_Node1_SerialDanger()
// {
//     serialPort->write("1ybj");
// }
// void taskserialport::Slot_Node1_SerialSafe()
// {
//     serialPort->write("1nbj");//调用串口对象（QSerialPort）的 write 方法，向连接的串口设备（如单片机、传感器等）发送字符串 "1nbj"，解除警报等
// }

// void taskserialport::Slot_Node2_SerialDanger()
// {
//     serialPort->write("2ybj");
// }
// void taskserialport::Slot_Node2_SerialSafe()
// {
//     serialPort->write("2nbj");
// }

// void taskserialport::Slot_Node3_SerialDanger()
// {
//     serialPort->write("3ybj");
// }
// void taskserialport::Slot_Node3_SerialSafe()
// {
//     serialPort->write("3nbj");
// }
