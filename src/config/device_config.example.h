#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <QString>

namespace DeviceConfig {
inline const QString kMqttDeviceId = QStringLiteral("replace-with-your-device-id");
inline const QString kMqttDeviceSecret = QStringLiteral("replace-with-your-device-secret");
inline const QString kMqttHostName = QStringLiteral("replace-with-your-mqtt-host");
inline const QString kMqttClientId = QStringLiteral("replace-with-your-client-id");
inline const QString kWifiAccessPointSsid = QStringLiteral("replace-with-your-wifi-ssid");
inline const QString kWifiAccessPointPassword = QStringLiteral("replace-with-your-wifi-password");
}

#endif // DEVICE_CONFIG_H
