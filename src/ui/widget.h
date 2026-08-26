#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QNetworkRequest>
#include <QUrl>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonParseError>

#include "mqtt/qmqtt.h"
#include <QMessageAuthenticationCode>//Qt的加密模块

#include <QtCharts/QChart>//图表容器（核心类）
#include <QtCharts/QSplineSeries>//平滑曲线序列（数据载体）
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QChartView>
#include <QDateTimeAxis>//日期时间轴（X轴）适用于时间序列数据（如实时监测数据）。
#include <QValueAxis>//数值轴（Y轴）适用于普通数值数据（如温度、湿度、光照强度）


#include <QTime>//读取时间
#include <QTimer>//定时器

#include <QThread>//qt线程

#include <QMessageBox>//窗口类
#include <QString>
#include <QSerialPortInfo>//串口信息类
#include <QSerialPort>
#include <QFont>
#include <QFile>
#include <QDir>
#include <QFileDialog>

#include "node1.h"
#include "node2.h"
#include "node3.h"
#include "setting.h"
#include "taskserialport.h"
#include "taskwarning.h"
#include "taskmqtt.h"

//曲线X,Y轴范围的最大值
#define MAX_X       9
#define MAX_Y       50
#define MAX_Y_HUMI  99
#define MAX_Y_LIGHT 2000
#define MAX_Y_SMOG 2000

//#define test

QT_CHARTS_USE_NAMESPACE //使用QtCharts命名空间
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    /* 线程 */
    QThread *threadSerial;
    QThread *threadMqtt;
    QThread *threadOneSecondTimer;
    QThread *threadWarning;//警告和信息显示线程

    QTimer *serialTimer;
    QTimer *oneSecondTimer;

signals:
    void Signal_SerialPortProcess();//串口处理信号

    void Signal_CurrentTime();


private slots:
    void on_node1Bt_clicked();

    void on_setBt_clicked();

    void Slot_OneSecondTimer();//定时器1s计时到 槽函数

    void Slot_Signal_SerialPortReadBuffer();//串口读取处理计时槽函数
    void Slot_SerialPortProcess();//串口信息处理函数

    void Slot_MqttConnectSuccess();//mqtt成功连接时，弹出成功
     void Slot_UIReturn();//返回函数
    void on_node2Bt_clicked();

     void on_node3Bt_clicked();
    // 节点下线检测函数
    void checkNodeOfflineStatus();
     void on_pushButton_clicked();

 private:
    Ui::Widget *ui;
     QNetworkAccessManager *manager;
};

// 节点下线检测相关变量
extern QDateTime lastDataTime_node1;  // 节点1最后一次接收数据的时间
extern QDateTime lastDataTime_node2;  // 节点2最后一次接收数据的时间
extern QDateTime lastDataTime_node3;  // 节点3最后一次接收数据的时间
extern int nodeOfflineTimeout_seconds; // 节点下线超时时间（秒）

extern QString serialPortReadBuffer;//声明外部变量
extern QStringList serialPortReadList;
extern QString stringTemp1; //温度
extern QString stringTemp2;
extern QString stringTemp3;
extern QString stringHumi1; //湿度
extern QString stringHumi2;
extern QString stringHumi3;
extern QString stringLight1; //光照
extern QString stringLight2;
extern QString stringLight3;
extern QString stringSmog1; //烟雾
extern QString stringSmog2;
extern QString stringSmog3;

extern QString stringNumber;
extern qreal nodeNumber;//节点号数


extern QDateTime currentDatetime;
extern QString stringCurrentTime;
extern QString stringCurrentTimeMessage;

extern bool state_nodeOne;
extern bool state_nodeTwo;
extern bool state_nodeThr;
extern bool flag1;
extern bool flag2;
extern bool flag3;

extern qreal temp_max;
extern qreal temp_min;
extern qreal humi_max;
extern qreal humi_min;
extern qreal light_max;
extern qreal light_min;
extern qreal smog_max;

extern qreal temp1;
extern qreal humi1;
extern qreal light1;
extern qreal temp2;
extern qreal humi2;
extern qreal light2;
extern qreal temp3;
extern qreal humi3;
extern qreal light3;
extern qreal smog1;
extern qreal smog2;
extern qreal smog3;

extern qreal puc_Temp[10];
extern qreal puc_Temp2[10];
extern qreal puc_Temp3[10];

extern int node_number;
extern int node_disnumber;

extern int error1, error2, error3, error4, error5, error6;
extern int error7, error8, error9, error10, error11, error12;
extern int error13, error14, error15;

/* part of charts */
extern QChart *chart_nodeOne;
extern QSplineSeries *Series_nodeOne_One;
extern QSplineSeries *Series_nodeOne_Two;
extern QSplineSeries *Series_nodeOne_Thr;
extern QSplineSeries *Series_nodeOne_Fou;
extern QDateTimeAxis *axisX_nodeOne;
extern QValueAxis *axisY_nodeOne_temp;
extern QValueAxis *axisY_nodeOne_humi;
extern QValueAxis *axisY_nodeOne_light;
extern QValueAxis *axisY_nodeOne_smog;

extern QChart *chart_nodeTwo;
extern QSplineSeries *Series_nodeTwo_One;
extern QSplineSeries *Series_nodeTwo_Two;
extern QSplineSeries *Series_nodeTwo_Thr;
extern QSplineSeries *Series_nodeTwo_Fou;
extern QDateTimeAxis *axisX_nodeTwo;
extern QValueAxis *axisY_nodeTwo;
extern QValueAxis *axisY_humi_nodeTwo;
extern QValueAxis *axisY_light_nodeTwo;
extern QValueAxis *axisY_smog_nodeTwo;


extern QChart *chart_nodeThr;
extern QSplineSeries *Series_nodeThr_One;
extern QSplineSeries *Series_nodeThr_Two;
extern QSplineSeries *Series_nodeThr_Thr;
extern QSplineSeries *Series_nodeThr_Fou;
extern QDateTimeAxis *axisX_nodeThr;
extern QValueAxis *axisY_nodeThr;
extern QValueAxis *axisY_humi_nodeThr;
extern QValueAxis *axisY_light_nodeThr;
extern QValueAxis *axisY_smog_nodeThr;

#endif // WIDGET_H
