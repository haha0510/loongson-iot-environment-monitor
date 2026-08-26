#include "widget.h"
#include "ui_widget.h"


/* Part of CurrentTime */
QDateTime currentDatetime;
QString stringCurrentTime;
QString stringCurrentTimeMessage;

bool state_nodeOne = false;//节点1是否上线
bool state_nodeTwo = false;//
bool state_nodeThr = false;


// 节点下线检测相关变量
QDateTime lastDataTime_node1;  // 节点1最后一次接收数据的时间
QDateTime lastDataTime_node2;  // 节点2最后一次接收数据的时间
QDateTime lastDataTime_node3;  // 节点3最后一次接收数据的时间
int nodeOfflineTimeout_seconds = 5; // 节点下线超时时间（秒），默认5秒


/* Part of SerialPort */
QString serialPortReadBuffer;//接受串口传来的数据的全局变量
QStringList serialPortReadList;
QString stringTemp1 = "0"; //温度
QString stringTemp2 = "0";
QString stringTemp3 = "0";
QString stringHumi1 = "0"; //湿度
QString stringHumi2 = "0";
QString stringHumi3 = "0";
QString stringLight1 = "0"; //光照
QString stringLight2 = "0";
QString stringLight3 = "0";
QString stringSmog1 = "0"; //烟雾
QString stringSmog2 = "0";
QString stringSmog3 = "0";

QString stringNumber;
qreal nodeNumber;//节点号数


/* Part of Charts */
QChart *chart_nodeOne;
QSplineSeries *Series_nodeOne_One;//节点1曲线序列
QSplineSeries *Series_nodeOne_Two;
QSplineSeries *Series_nodeOne_Thr;
QSplineSeries *Series_nodeOne_Fou;
QDateTimeAxis *axisX_nodeOne;   //X轴为时间轴
QValueAxis *axisY_nodeOne_temp; //Y轴为温度轴
QValueAxis *axisY_nodeOne_humi; //Y轴为湿度轴
QValueAxis *axisY_nodeOne_light;//Y轴为光照轴
QValueAxis *axisY_nodeOne_smog;//Y烟雾浓度轴

QChart *chart_nodeTwo;
QSplineSeries *Series_nodeTwo_One;
QSplineSeries *Series_nodeTwo_Two;
QSplineSeries *Series_nodeTwo_Thr;
QSplineSeries *Series_nodeTwo_Fou;
QDateTimeAxis *axisX_nodeTwo;
QValueAxis *axisY_nodeTwo;
QValueAxis *axisY_humi_nodeTwo;
QValueAxis *axisY_light_nodeTwo;
QValueAxis *axisY_smog_nodeTwo;

QChart *chart_nodeThr;
QSplineSeries *Series_nodeThr_One;
QSplineSeries *Series_nodeThr_Two;
QSplineSeries *Series_nodeThr_Thr;
QSplineSeries *Series_nodeThr_Fou;
QDateTimeAxis *axisX_nodeThr;
QValueAxis *axisY_nodeThr;
QValueAxis *axisY_humi_nodeThr;
QValueAxis *axisY_light_nodeThr;
QValueAxis *axisY_smog_nodeThr;

/* Part of Data */
qreal temp_max = 50;//设置的上下限值
qreal temp_min = 10;
qreal humi_max = 80;
qreal humi_min = 10;
qreal light_max = 1000;
qreal light_min = 10;
qreal smog_max = 800;

qreal temp1;//数据值
qreal humi1;
qreal light1;
qreal temp2;
qreal humi2;
qreal light2;
qreal temp3;
qreal humi3;
qreal light3;
qreal smog1;
qreal smog2;
qreal smog3;

bool flag1=1;
bool flag2=1;
bool flag3=1;

int node_number;
int node_disnumber = 3;//没有上线的节点数

//unsigned int error_index = 0x0000;
int error1, error2, error3, error4, error5, error6;
int error7, error8, error9, error10, error11, error12;
int error13, error14, error15;

/*part of ui*/
node1 *node1_ui;//定义ui指针变量，在widget构造函数里赋值
node2 *node2_ui;
node3 *node3_ui;

setting *setting_ui;


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    currentDatetime = QDateTime::currentDateTime();
    stringCurrentTime = currentDatetime.toString("yyyy-MM-dd hh:mm:ss ddd");
    stringCurrentTimeMessage = currentDatetime.toString("yyyy-MM-dd hh:mm:ss ");
    ui->label_time->setText(stringCurrentTime);  //获取时间显示，该读取一次

    manager = new QNetworkAccessManager;

    /* Part of Serial Prot */
    taskserialport *mySerialPort = new taskserialport();
    threadSerial = new QThread();//串口线程
    connect(threadSerial,&QThread::started,mySerialPort,&taskserialport::Init_SerialPort);//将连接线程启动信号与串口初始化函数连接，只要线程一启动就初始化串口
    mySerialPort->moveToThread(threadSerial);
    threadSerial->start();//启动线程

    connect(mySerialPort,&taskserialport::Signal_SerialPortReadBuffer,this,&Widget::Slot_Signal_SerialPortReadBuffer);
                //一收到串口信息就启动50ms间隔处理信息

    /*node_1*/
    node1_ui = new node1;
    node1_ui->hide();
    connect(node1_ui, SIGNAL(Signal_Return()), this, SLOT(Slot_UIReturn()));//ui返回按钮
    connect(this, SIGNAL(Signal_CurrentTime()), node1_ui, SLOT(Slot_RefreshTime()));//时间显示
    //connect(node1_ui, SIGNAL(Node1_Safe()),mySerialPort, SLOT(Slot_Node1_SerialSafe()));//节点1解除警报
    //connect(node1_ui, SIGNAL(Node1_Danger()),mySerialPort, SLOT(Slot_Node1_SerialDanger()));//发出警报，发出串口信息控制蜂鸣器响

    /*node_2*/
    node2_ui = new node2;
    node2_ui->hide();
    connect(node2_ui, SIGNAL(Signal_Return()), this, SLOT(Slot_UIReturn()));
    connect(this, SIGNAL(Signal_CurrentTime()), node2_ui, SLOT(Slot_RefreshTime()));
    //connect(node2_ui, SIGNAL(Node2_Safe()),mySerialPort, SLOT(Slot_Node2_SerialSafe()));
    //connect(node2_ui, SIGNAL(Node2_Danger()),mySerialPort, SLOT(Slot_Node2_SerialDanger()));

    /*node_3*/
    node3_ui = new node3;
    node3_ui->hide();
    connect(node3_ui, SIGNAL(Signal_Return()), this, SLOT(Slot_UIReturn()));
    connect(this, SIGNAL(Signal_CurrentTime()), node3_ui, SLOT(Slot_RefreshTime()));
    //connect(node3_ui, SIGNAL(Node3_Safe()),mySerialPort, SLOT(Slot_Node3_SerialSafe()));
    //connect(node3_ui, SIGNAL(Node3_Danger()),mySerialPort, SLOT(Slot_Node3_SerialDanger()));


    /* Part of MQTT */
    taskMqtt *myMqtt = new taskMqtt;
    //threadMqtt = new QThread();
    myMqtt->Init_MQTT();
    //myMqtt->moveToThread(threadMqtt);
    //threadMqtt->start();
    connect(myMqtt, SIGNAL(Signal_MqttConnectSuccess()), this, SLOT(Slot_MqttConnectSuccess()));//显示mqtt连接成功
    connect(this, &Widget::Signal_SerialPortProcess, myMqtt, &taskMqtt::TaskSendToCloud);//上云


    serialTimer = new QTimer(this);//定义串口计时
    connect(serialTimer,&QTimer::timeout,this,&Widget::Slot_SerialPortProcess);//50ms计时到，处理串口信息，转到myWarning->TaskWarning();

    /* Part of Timer */
    threadOneSecondTimer = new QThread();
    oneSecondTimer = new QTimer();//定时器定义
    oneSecondTimer->moveToThread(threadOneSecondTimer);//将定时器依附到子线程
    oneSecondTimer->setInterval(1000);//设置定时器间隔

    /*子线程启动后（started() 信号发射），再启动定时器（调用 start()）*/
    connect(threadOneSecondTimer, SIGNAL(started()), oneSecondTimer, SLOT(start()));
     threadOneSecondTimer->start();//开启子线程

    /*定时器超时处理，timeout() 信号在子线程发出，但 Slot_OneSecondTimer() 默认在主线程执行*/
    connect(oneSecondTimer, SIGNAL(timeout()), this, SLOT(Slot_OneSecondTimer()));//1s间隔执行Slot_OneSecondTimer()


    /*set*/
    setting_ui = new setting;
    setting_ui->hide();
    connect(setting_ui, SIGNAL(Signal_Return()), this, SLOT(Slot_UIReturn()));
    connect(this, SIGNAL(Signal_CurrentTime()), setting_ui, SLOT(Slot_RefreshTime()));


    /* Part of Warning */
    taskwarning *myWarning = new taskwarning;
    threadWarning = new QThread();
    myWarning->moveToThread(threadWarning);
    threadWarning->start();
    connect(this, &Widget::Signal_SerialPortProcess, myWarning, [=]
            {
                myWarning->TaskWarning();
            });//将myWarning的TAskWarning()函数作为槽函数连接
    connect(myWarning, SIGNAL(Signal_Warning(uint)), node1_ui, SLOT(Slot_Warning(uint))); //节点ui显示数据,Qt::QueuedConnection(保证在主线程执行)
    connect(myWarning, SIGNAL(Signal_Warning(uint)), node2_ui, SLOT(Slot_Warning(uint))); //node2显示数据----
    connect(myWarning, SIGNAL(Signal_Warning(uint)), node3_ui, SLOT(Slot_Warning(uint)));//node3显示数据----

}

Widget::~Widget()
{
    delete ui;
}


void Widget::Slot_OneSecondTimer()
{
    currentDatetime = QDateTime::currentDateTime();
    stringCurrentTime = currentDatetime.toString("yyyy-MM-dd hh:mm:ss ddd");
    stringCurrentTimeMessage = currentDatetime.toString("yyyy-MM-dd hh:mm:ss ");

    //设置坐标轴显示范围
    chart_nodeOne->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60 * 1));       //系统当前时间的前一秒
    chart_nodeOne->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));            //系统当前时间
    chart_nodeTwo->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60 * 1));       //系统当前时间的前一秒
    chart_nodeTwo->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));            //系统当前时间
    chart_nodeThr->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60 * 1));       //系统当前时间的前一秒
    chart_nodeThr->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));            //系统当前时间


    if(state_nodeOne)
    {
        Series_nodeOne_One->append(currentDatetime.toMSecsSinceEpoch(), temp1);
        Series_nodeOne_Two->append(currentDatetime.toMSecsSinceEpoch(), humi1);
        Series_nodeOne_Thr->append(currentDatetime.toMSecsSinceEpoch(), light1);
        Series_nodeOne_Fou->append(currentDatetime.toMSecsSinceEpoch(), smog1);
    }
    if(state_nodeTwo)
    {
        Series_nodeTwo_One->append(currentDatetime.toMSecsSinceEpoch(), temp2);
        Series_nodeTwo_Two->append(currentDatetime.toMSecsSinceEpoch(), humi2);
        Series_nodeTwo_Thr->append(currentDatetime.toMSecsSinceEpoch(), light2);
        Series_nodeTwo_Fou->append(currentDatetime.toMSecsSinceEpoch(), smog2);
    }
    if(state_nodeThr)
    {
        Series_nodeThr_One->append(currentDatetime.toMSecsSinceEpoch(), temp3);
        Series_nodeThr_Two->append(currentDatetime.toMSecsSinceEpoch(), humi3);
        Series_nodeThr_Thr->append(currentDatetime.toMSecsSinceEpoch(), light3);
        Series_nodeThr_Fou->append(currentDatetime.toMSecsSinceEpoch(), smog3);
    }

    ui->label_time->setText(stringCurrentTime);

    // 执行节点下线检测
    checkNodeOfflineStatus();
    node1_ui->downline();
    node2_ui->downline();
    node3_ui->downline();
     emit Signal_CurrentTime();
}


void Widget::Slot_Signal_SerialPortReadBuffer() //启动50ms计时，50ms处理一次串口信息
{
    serialTimer->start(50); //超过50ms，发出timeout信号
}

void Widget::Slot_SerialPortProcess()//串口信息处理函数（在主线程中）
{
    //停止定时器（防止处理过程中新数据干扰）
    serialTimer->stop();
    // 调试输出原始数据
   // qDebug() << serialPortReadBuffer;

    // 1. 检查数据是否包含 "+IPD" 前缀
    if (serialPortReadBuffer.contains("+IPD"))
    {
        // 2. 提取冒号后面的数据部分
        int colonIndex = serialPortReadBuffer.indexOf(':');//找到冒号的位置
        if (colonIndex != -1)
        {
            QString dataPart = serialPortReadBuffer.mid(colonIndex + 1);//提取冒号后面的数据部分

            // 3. 去除可能的回车换行符
            dataPart = dataPart.trimmed();
            dataPart = dataPart.replace("\r\n", "");

            // 4. 分割数据字段
            serialPortReadList = dataPart.split(",");

            // 调试输出解析过程
            // qDebug() << "Data part:" << dataPart;
            // qDebug() << "Data fields:" << serialPortReadList;//dataFields;

            // 5. 验证数据完整性（应有5个字段）
            // 提取节点号（第一个字段）
            stringNumber = serialPortReadList[0];
            nodeNumber = stringNumber.toDouble();

     // 校验节点号和数据完整性（假设节点1有5个字段）

            if(nodeNumber == 1 && serialPortReadList.size() == 5)//
            {
                stringLight1 = serialPortReadList[1];//光照
                stringTemp1 = serialPortReadList[2]; //温度
                stringHumi1 = serialPortReadList[3]; //湿度
                stringSmog1 = serialPortReadList[4]; //烟雾

                // 转换为数值类型
                temp1 = stringTemp1.toDouble();
                humi1 = stringHumi1.toDouble();
                light1 = stringLight1.toDouble();
                smog1 = stringSmog1.toDouble();

                // 更新节点1最后接收数据的时间戳
                lastDataTime_node1 = QDateTime::currentDateTime();

                qDebug() << "Node 1 data:" << nodeNumber << temp1 << humi1 << light1 << smog1;
                qDebug() <<"string"<<stringTemp1<< stringHumi1<<stringLight1<<stringSmog1;
            }
            else if(nodeNumber == 2&&serialPortReadList.size() == 5)
            {
                stringLight2 = serialPortReadList[1];//光照
                stringTemp2 = serialPortReadList[2]; //温度
                stringHumi2 = serialPortReadList[3]; //湿度
                stringSmog2 = serialPortReadList[4]; //烟雾
                temp2 = stringTemp2.toDouble();
                humi2 = stringHumi2.toDouble();
                light2 = stringLight2.toDouble();
                smog2 = stringSmog2.toDouble();

                // 更新节点2最后接收数据的时间戳
                lastDataTime_node2 = QDateTime::currentDateTime();

                qDebug() << "Node 2 data:" <<nodeNumber<< temp2 << humi2 << light2 << smog2;
            }
            else if(nodeNumber == 3&&serialPortReadList.size() == 5)
            {
                stringLight3 = serialPortReadList[1];//光照
                stringTemp3 = serialPortReadList[2]; //温度
                stringHumi3 = serialPortReadList[3]; //湿度
                stringSmog3 = serialPortReadList[4]; //烟雾
                temp3 = stringTemp3.toDouble();
                humi3 = stringHumi3.toDouble();
                light3 = stringLight3.toDouble();
                smog3 = stringSmog3.toDouble();

                // 更新节点3最后接收数据的时间戳
                lastDataTime_node3 = QDateTime::currentDateTime();

                qDebug() << "Node 3 data:" <<nodeNumber<< temp3 << humi3 << light3 << smog3;
            }


        }

    }

    emit Signal_SerialPortProcess();
    serialPortReadBuffer.clear();
}

void Widget::Slot_UIReturn()
{
    node1_ui->hide();
    node2_ui->hide();
    node3_ui->hide();
    setting_ui->hide();

}

// 节点下线检测函数
void Widget::checkNodeOfflineStatus()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    // 检测节点1
    if (lastDataTime_node1.isValid()) // 如果有过数据接收记录
    {
        qint64 secondsSinceLastData = lastDataTime_node1.secsTo(currentTime);
        if (secondsSinceLastData > nodeOfflineTimeout_seconds)
        {
            // 节点1超时，设置为下线
            if (state_nodeOne) // 如果之前是在线状态
            {

                qDebug() << "Node 1 offline detected! Last data received" << secondsSinceLastData << "seconds ago";

                // 清空节点1的数据
                temp1 = 0;
                humi1 = 0;
                light1 = 0;
                smog1 = 0;
                stringTemp1 = "0";  stringHumi1 = "0"; stringLight1 = "0"; stringSmog1 = "0";

                state_nodeOne = 0;

            }
        }
        else
        {
            // 节点1在线，检查数据有效性
            if ((temp1 != 0) || (humi1 != 0) || (light1 != 0) || (smog1 != 0))
            {
                state_nodeOne = true;
            }
        }
    }

    // 检测节点2
    if (lastDataTime_node2.isValid()) // 如果有过数据接收记录
    {
        qint64 secondsSinceLastData = lastDataTime_node2.secsTo(currentTime);
        if (secondsSinceLastData > nodeOfflineTimeout_seconds)
        {
            // 节点2超时，设置为下线
            if (state_nodeTwo) // 如果之前是在线状态
            {
                state_nodeTwo = false;
                qDebug() << "Node 2 offline detected! Last data received" << secondsSinceLastData << "seconds ago";

                // 清空节点2的数据
                temp2 = 0;
                humi2 = 0;
                light2 = 0;
                smog2 = 0;
                stringTemp2 = "0";  stringHumi2 = "0"; stringLight2 = "0"; stringSmog2 = "0";
            }
        }
        else
        {
            // 节点2在线，检查数据有效性
            if ((temp2 != 0) || (humi2 != 0) || (light2 != 0) || (smog2 != 0))
            {
                state_nodeTwo = true;
            }
        }
    }

    // 检测节点3
    if (lastDataTime_node3.isValid()) // 如果有过数据接收记录
    {
        qint64 secondsSinceLastData = lastDataTime_node3.secsTo(currentTime);
        if (secondsSinceLastData > nodeOfflineTimeout_seconds)
        {
            // 节点3超时，设置为下线
            if (state_nodeThr) // 如果之前是在线状态
            {
                state_nodeThr = false;
                qDebug() << "Node 3 offline detected! Last data received" << secondsSinceLastData << "seconds ago";

                // 清空节点3的数据
                temp3 = 0;
                humi3 = 0;
                light3 = 0;
                smog3 = 0;
                stringTemp3 = "0";  stringHumi3 = "0"; stringLight3 = "0"; stringSmog3 = "0";
            }
        }
        else
        {
            // 节点3在线，检查数据有效性
            if ((temp3 != 0) || (humi3 != 0) || (light3 != 0) || (smog3 != 0))
            {
                state_nodeThr = true;
            }
        }
    }
}


void Widget::Slot_MqttConnectSuccess()
{
    QMessageBox::information(this, "info", "MqttConnectSuccess!!!");
}
void Widget::on_node1Bt_clicked()
{

    node1_ui->setGeometry(this->geometry());//获得主窗口的宽高
    node1_ui->show();
}


void Widget::on_setBt_clicked()
{

    //setting_ui->setGeometry(this->geometry());//获得主窗口的宽高
    setting_ui->show();

    //进来时需要获取警告值
}

void Widget::on_node2Bt_clicked()
{

    node2_ui->setGeometry(this->geometry());//获得主窗口的宽高
    node2_ui->show();
}


void Widget::on_node3Bt_clicked()
{
    // ui->node1Bt->hide();
    // ui->node2Bt->hide();
    // ui->node3Bt->hide();
    // ui->setBt->hide();
    // ui->label->hide();
    // ui->label_2->hide();
    // ui->label_3->hide();
    // ui->label_4->hide();

    node3_ui->setGeometry(this->geometry());//获得主窗口的宽高
    node3_ui->show();
}


// void Widget::on_pushButton_clicked()
// {



//     QString test = ui->inputEdit->toPlainText();

//     ui->inputEdit->clear();

//     ui->outPutEdit->insertPlainText("user:");
//     ui->outPutEdit->insertPlainText(test);
//     ui->outPutEdit->insertPlainText("\n\n");

//     QNetworkRequest request;
//     request.setUrl(QUrl("https://api.deepseek.com/chat/completions"));
//     request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
//     request.setRawHeader("Accept","application/json");
//     request.setRawHeader("Authorization","Bearer sk-69ca99b4265342d395ca702684102d1f");

//     QJsonArray messages;

//     QJsonObject systemMsg;
//     systemMsg["role"] = "system";
//     systemMsg["content"] = "You are a helpful assistant";

//     QJsonObject userMsg;
//     userMsg["role"] = "user";
//     userMsg["content"] = test;//发送的数据

//     messages.append(systemMsg);
//     messages.append(userMsg);

//     QJsonObject requsestBody;
//     requsestBody["messages"] = messages;
//     requsestBody["model"] = "deepseek-chat";
//     requsestBody["max_tokens"] = 2048;
//     requsestBody["stream"] = true;
//     requsestBody["temperature"] = 1;//思考深度

//     //发送请求
//     QNetworkReply *reply = manager->post(request,QJsonDocument(requsestBody).toJson());


//     //处理数据
//     connect(reply,&QNetworkReply::readyRead,this,[=]{
//         while(reply->canReadLine())
//         {
//             QString line = reply->readLine().trimmed();//去掉换行符

//             if(line.startsWith("data:"))//如果是一个以data开头的数据就留下来
//             {
//                 line.remove(0,6); //去掉前六个

//                 QJsonParseError error;

//                 QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(),&error);

//                 if(error.error == QJsonParseError::NoError)//判断是否解析成功, 是个Json对象
//                 {
//                     QString content = doc.object()["choices"].toArray().first()
//                                           .toObject()["delta"].toObject()["content"].toString();

//                     if(!content.isEmpty())//如果不为空，则显示
//                     {
//                         ui->outPutEdit->moveCursor(QTextCursor::End);
//                         ui->outPutEdit->insertPlainText(content);
//                     }
//                 }
//             }
//         }
//     });

//     connect(reply,&QNetworkReply::finished,this,[=]{
//         ui->outPutEdit->moveCursor(QTextCursor::End);
//         ui->outPutEdit->insertPlainText("\n\n");//输出结束换行

//         reply->deleteLater();
//     });
// }


void Widget::on_pushButton_clicked()
{
    QString test = ui->inputEdit->toPlainText().trimmed();
    ui->inputEdit->clear();

    ui->outPutEdit->insertPlainText("user--> ");
    ui->outPutEdit->insertPlainText(test);
    ui->outPutEdit->insertPlainText("\n\n");

    // 检查是否包含节点查询关键字
    if (test.contains("节点状态", Qt::CaseInsensitive) ||
        test.contains("节点情况", Qt::CaseInsensitive) ||
        test.contains("查看节点", Qt::CaseInsensitive)) {

        // 开始构建节点状态响应
        QString response = "assistant: 当前节点状态如下：\n";
        response += "--------------------------------\n";

        // 检查节点1状态
        if (stringTemp1 == "0" && stringHumi1 == "0" &&
            stringLight1 == "0" && stringSmog1 == "0") {
            response += "节点1: 未上线\n";
        } else {
            response += QString("节点1: 温度=%1℃, 湿度=%2%%, 光照=%3Lux, 烟雾=%4ppm\n")
                            .arg(stringTemp1).arg(stringHumi1).arg(stringLight1).arg(stringSmog1);
        }

        // 检查节点2状态
        if (stringTemp2 == "0" && stringHumi2 == "0" &&
            stringLight2 == "0" && stringSmog2 == "0") {
            response += "节点2: 未上线\n";
        } else {
            response += QString("节点2: 温度=%1℃, 湿度=%2%%, 光照=%3Lux, 烟雾=%4ppm\n")
                            .arg(stringTemp2).arg(stringHumi2).arg(stringLight2).arg(stringSmog2);
        }

        // 检查节点3状态
        if (stringTemp3 == "0" && stringHumi3 == "0" &&
            stringLight3 == "0" && stringSmog3 == "0") {
            response += "节点3: 未上线\n";
        } else {
            response += QString("节点3: 温度=%1℃, 湿度=%2%%, 光照=%3Lux, 烟雾=%4ppm\n")
                            .arg(stringTemp3).arg(stringHumi3).arg(stringLight3).arg(stringSmog3);
        }

        response += "--------------------------------\n";
        ui->outPutEdit->insertPlainText(response);
        ui->outPutEdit->insertPlainText("\n\n");
        return;  // 直接返回，不调用API
    }

    // 普通聊天请求处理
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.deepseek.com/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Authorization", "Bearer sk-69ca99b4265342d395ca702684102d1f");

    QJsonArray messages;

    // 添加系统提示词（包含节点状态上下文）
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "你是一个物联网监测助手，可以查询三个环境监测节点的状态。"
                           "当用户询问节点状态时，请引导用户使用'查询节点状态'指令。"
                           "节点数据格式：温度(℃)、湿度(%)、光照(Lux)、烟雾(ppm)";

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = test;

    messages.append(systemMsg);
    messages.append(userMsg);

    QJsonObject requestBody;
    requestBody["messages"] = messages;
    requestBody["model"] = "deepseek-chat";
    requestBody["max_tokens"] = 2048;
    requestBody["stream"] = true;
    requestBody["temperature"] = 1;  // 稍低的温度值提高确定性

    // 发送请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(requestBody).toJson());

    // 处理流式响应
    connect(reply, &QNetworkReply::readyRead, this, [=]{
        while(reply->canReadLine()) {
            QString line = reply->readLine().trimmed();
            if(line.startsWith("data:")) {
                line.remove(0, 5); // 移除"data:"前缀

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);

                if(error.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    if(obj.contains("choices")) {
                        QJsonArray choices = obj["choices"].toArray();
                        if(!choices.isEmpty()) {
                            QJsonObject choice = choices[0].toObject();
                            if(choice.contains("delta")) {
                                QJsonObject delta = choice["delta"].toObject();
                                if(delta.contains("content")) {
                                    QString content = delta["content"].toString();
                                    if(!content.isEmpty()) {
                                        ui->outPutEdit->moveCursor(QTextCursor::End);
                                        ui->outPutEdit->insertPlainText(content);
                                        QApplication::processEvents(); // 确保UI及时更新
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    });

    // 请求结束时处理
    connect(reply, &QNetworkReply::finished, this, [=]{
        ui->outPutEdit->moveCursor(QTextCursor::End);
        ui->outPutEdit->insertPlainText("\n\n");
        reply->deleteLater();
    });
}

