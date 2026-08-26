#ifndef TASKMQTT_H
#define TASKMQTT_H

#include <QObject>
#include "widget.h"

class taskMqtt : public QObject
{
    Q_OBJECT
public:
    explicit taskMqtt(QObject *parent = nullptr);

    QMQTT::Client *mqttClient;
    QString m_strPubTopic, m_strSubTopic;//Pub：发布主题，Sub：订阅主题
    void Init_MQTT();
    void Init_HuaweiCloud_MQTT();
signals:
     void Signal_MqttConnectSuccess();

public slots:
     void Slot_MqttConnectSuccess();

     void TaskSendToCloud();
};

#endif // TASKMQTT_H
