#ifndef TASKPWM_H
#define TASKPWM_H

#include <QObject>

class QTimer;

/* 超阈值呼吸灯 —— 龙芯板载 PWM(标准 Linux /sys/class/pwm)。
 * 收到告警(error_index!=0)启动呼吸(占空比正弦渐变)，恢复(==0)则熄灭。
 * pwmchip/通道号在 .cpp 顶部 #define，上板前用 `ls /sys/class/pwm/` 确认。
 * 仅 Linux 有效；其它平台空操作，保证 Windows 也能编译。 */
class taskpwm : public QObject
{
    Q_OBJECT
public:
    explicit taskpwm(QObject *parent = nullptr);
    ~taskpwm();

public slots:
    void Init();                                 // 线程启动后：导出并配置 PWM 通道
    void Slot_Alarm(unsigned int error_index);   // 告警位掩码：!=0 呼吸，==0 熄灭

private slots:
    void breatheStep();                          // 定时推进呼吸相位

private:
    QTimer *timer_ = nullptr;
    bool    ready_ = false;         // PWM 是否初始化成功
    bool    alarming_ = false;      // 当前是否处于呼吸状态
    double  phase_ = 0.0;           // 呼吸相位
    long    periodNs_ = 1000000;    // PWM 周期 1ms(1kHz)

    bool exportChannel();
    void writeAttr(const QString &attr, const QString &val);
    void setDuty(long dutyNs);
    void setEnable(bool on);
};

#endif // TASKPWM_H
