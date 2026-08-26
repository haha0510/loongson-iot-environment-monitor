#include "taskmqtt.h"

taskMqtt::taskMqtt(QObject *parent)
    : QObject{parent}
{

}



void taskMqtt::TaskSendToCloud()
{
    int num_light1 = stringLight1.toInt();
    int num_temp1 = stringTemp1.toInt();
    int num_humi1 = stringHumi1.toInt();
    int num_smog1 = stringSmog1.toInt();
    int num_light2 = stringLight2.toInt();
    int num_temp2 = stringTemp2.toInt();
    int num_humi2 = stringHumi2.toInt();
    int num_smog2 = stringSmog2.toInt();
    int num_light3 = stringLight3.toInt();
    int num_temp3 = stringTemp3.toInt();
    int num_humi3 = stringHumi3.toInt();
    int num_smog3 = stringSmog3.toInt();

    QString temp_light1 = QString::number(num_light1);
    QString temp_Temp1 = QString::number(num_temp1);
    QString temp_Humi1 = QString::number(num_humi1);
    QString temp_Smog1 = QString::number(num_smog1);

    QString temp_light2 = QString::number(num_light2);
    QString temp_Temp2 = QString::number(num_temp2);
    QString temp_Humi2 = QString::number(num_humi2);
    QString temp_Smog2 = QString::number(num_smog2);

    QString temp_light3 = QString::number(num_light3);
    QString temp_Temp3 = QString::number(num_temp3);
    QString temp_Humi3 = QString::number(num_humi3);
    QString temp_Smog3 = QString::number(num_smog3);

    QString mqttMessage;

    mqttMessage = "{"
                  "\"services\": ["
                  "{"
                  "\"service_id\": \"loongson\","
                  "\"properties\": {"
                  "\"Node_1_Light\": " + temp_light1 + ","     // 节点一温度
                                  "\"Node_1_Temperature\": " + temp_Temp1 + ","        // 节点一湿度
                                  "\"Node_1_Humidity\": " + temp_Humi1 + ","          // 节点一光照
                                   "\"Node_1_Smog\": " + temp_Smog1 + ","              // 节点一烟雾浓度
                                 "\"Node_2_Light\": " + temp_light2 + ","
                                 "\"Node_2_Temperature\": " + temp_Temp2 + ","
                                 "\"Node_2_Humidity\": " + temp_Humi2 + ","
                                 "\"Node_2_Smog\": " + temp_Smog2 + ","
                                 "\"Node_3_Light\": " + temp_light3 + ","
                                 "\"Node_3_Temperature\": " + temp_Temp3 + ","
                                 "\"Node_3_Humidity\": " + temp_Humi3 + ","
                                 "\"Node_3_Smog\": " + temp_Smog3 +

                  "}"
                  "}"
                  "]"
                  "}";

    QMQTT::Message send_msg(136, m_strPubTopic, mqttMessage.toLocal8Bit(),0);
    mqttClient->publish(send_msg);

}

void taskMqtt::Init_MQTT()
{
    mqttClient = new QMQTT::Client;

    // 华为云IoT平台MQTT连接配置
    Init_HuaweiCloud_MQTT();
}
void taskMqtt::Init_HuaweiCloud_MQTT()
{
    // ========== 华为云IoT平台连接参数配置接口 ==========
    // 请在此处填入您的华为云IoT设备连接信息

    QString m_strDeviceId = "685a734ad582f2001834985f_loong_1";           // 设备ID - 请填入您的设备ID
    QString m_strDeviceSecret = "3246a4eab894b351f7d199a8d2f9aa302fece2d1204e126c429e044f1a1a082a";       // 设备密钥 - 请填入您的设备密钥

    // 华为云IoT平台MQTT服务器地址格式
    QString m_strHostName = "5930c00e73.st1.iotda-device.cn-north-4.myhuaweicloud.com";

    // 华为云IoT平台主题配置
    m_strPubTopic = "$oc/devices/" + m_strDeviceId + "/sys/properties/report";     // 属性上报主题
    m_strSubTopic = "$oc/devices/" + m_strDeviceId + "/sys/properties/set";        // 属性设置主题

    // 客户端ID配置
    QString clientId = "685a734ad582f2001834985f_loong_1_0_0_2025062508";

    // MQTT连接参数设置
    mqttClient->setHostName(m_strHostName);     // 设置服务器地址
    mqttClient->setPort(1883);                  // 华为云IoT平台MQTT SSL端口
    mqttClient->setUsername(m_strDeviceId);     // 用户名为设备ID
    mqttClient->setPassword(m_strDeviceSecret.toLocal8Bit()); // 密码为设备密钥
    mqttClient->setClientId(clientId);          // 客户端ID
    mqttClient->setKeepAlive(120);              // 心跳间隔120秒

    // 绑定连接成功槽函数
    connect(mqttClient, SIGNAL(connected()), this, SLOT(Slot_MqttConnectSuccess()));

    // 连接到华为云IoT平台
    mqttClient->connectToHost();
}

void taskMqtt::Slot_MqttConnectSuccess()
{
    emit Signal_MqttConnectSuccess();
}
