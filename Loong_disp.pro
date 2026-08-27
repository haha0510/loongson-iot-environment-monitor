QT += core gui charts network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += \
    $$PWD/src/ui \
    $$PWD/src/services \
    $$PWD/src/config \
    $$PWD/third_party

SOURCES += \
    src/main.cpp \
    src/ui/node1.cpp \
    src/ui/node2.cpp \
    src/ui/node3.cpp \
    src/ui/setting.cpp \
    src/ui/widget.cpp \
    src/services/taskmqtt.cpp \
    src/services/taskserialport.cpp \
    src/services/taskwarning.cpp \
    src/services/taskpwm.cpp \
    src/services/tasksysmonitor.cpp \
    src/services/taskdiagnosis.cpp \
    third_party/mqtt/qmqtt_client.cpp \
    third_party/mqtt/qmqtt_client_p.cpp \
    third_party/mqtt/qmqtt_frame.cpp \
    third_party/mqtt/qmqtt_message.cpp \
    third_party/mqtt/qmqtt_network.cpp \
    third_party/mqtt/qmqtt_router.cpp \
    third_party/mqtt/qmqtt_socket.cpp \
    third_party/mqtt/qmqtt_ssl_socket.cpp \
    third_party/mqtt/qmqtt_timer.cpp \
    third_party/mqtt/qmqtt_websocket.cpp \
    third_party/mqtt/qmqtt_websocketiodevice.cpp

HEADERS += \
    src/ui/node1.h \
    src/ui/node2.h \
    src/ui/node3.h \
    src/ui/setting.h \
    src/ui/widget.h \
    src/services/taskmqtt.h \
    src/services/taskserialport.h \
    src/services/taskwarning.h \
    src/services/taskpwm.h \
    src/services/tasksysmonitor.h \
    src/services/taskdiagnosis.h \
    src/config/device_config.example.h \
    third_party/mqtt/qmqtt.h \
    third_party/mqtt/qmqtt_client.h \
    third_party/mqtt/qmqtt_client_p.h \
    third_party/mqtt/qmqtt_frame.h \
    third_party/mqtt/qmqtt_global.h \
    third_party/mqtt/qmqtt_message.h \
    third_party/mqtt/qmqtt_message_p.h \
    third_party/mqtt/qmqtt_network_p.h \
    third_party/mqtt/qmqtt_networkinterface.h \
    third_party/mqtt/qmqtt_routedmessage.h \
    third_party/mqtt/qmqtt_router.h \
    third_party/mqtt/qmqtt_routesubscription.h \
    third_party/mqtt/qmqtt_socket_p.h \
    third_party/mqtt/qmqtt_socketinterface.h \
    third_party/mqtt/qmqtt_ssl_socket_p.h \
    third_party/mqtt/qmqtt_timer_p.h \
    third_party/mqtt/qmqtt_timerinterface.h \
    third_party/mqtt/qmqtt_websocket_p.h \
    third_party/mqtt/qmqtt_websocketiodevice_p.h

FORMS += \
    src/ui/node1.ui \
    src/ui/node2.ui \
    src/ui/node3.ui \
    src/ui/setting.ui \
    src/ui/widget.ui

SUBDIRS += \
    third_party/mqtt/qmqtt.pro

DISTFILES += \
    third_party/mqtt/qmqtt.pri \
    third_party/mqtt/qmqtt.qbs

RESOURCES += \
    resources/pic/pic.qrc
