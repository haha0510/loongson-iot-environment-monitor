#include "taskpwm.h"
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <math.h>

/* 可调：pwmchip 与通道号。上板前用 `ls /sys/class/pwm/` 确认存在哪个 pwmchip，
 * 以及目标 LED 接在哪一路（并确保该引脚的 pwm_sel 已复用为 PWM 功能）。*/
#define PWM_CHIP      "pwmchip0"
#define PWM_CHANNEL   0
#define PWM_BREATH_MS 30          /* 呼吸刷新间隔(ms) */

static const double kPi = 3.14159265358979323846;

static QString chipBase() { return QString("/sys/class/pwm/%1").arg(PWM_CHIP); }
static QString chanBase() { return QString("/sys/class/pwm/%1/pwm%2")
                                       .arg(PWM_CHIP).arg(PWM_CHANNEL); }

taskpwm::taskpwm(QObject *parent) : QObject{parent} {}

taskpwm::~taskpwm()
{
    if (ready_) setEnable(false);   /* 退出时熄灯，尽力而为 */
}

void taskpwm::writeAttr(const QString &attr, const QString &val)
{
#ifdef Q_OS_LINUX
    QFile f(attr);
    if (f.open(QIODevice::WriteOnly)) { f.write(val.toUtf8()); f.close(); }
#else
    Q_UNUSED(attr); Q_UNUSED(val);
#endif
}

bool taskpwm::exportChannel()
{
#ifdef Q_OS_LINUX
    if (!QFile::exists(chanBase()))
        writeAttr(chipBase() + "/export", QString::number(PWM_CHANNEL));
    return QFile::exists(chanBase());
#else
    return false;
#endif
}

void taskpwm::setDuty(long dutyNs)
{
    if (dutyNs < 0) dutyNs = 0;
    if (dutyNs > periodNs_) dutyNs = periodNs_;
    writeAttr(chanBase() + "/duty_cycle", QString::number(dutyNs));
}

void taskpwm::setEnable(bool on)
{
    writeAttr(chanBase() + "/enable", on ? "1" : "0");
}

void taskpwm::Init()
{
#ifdef Q_OS_LINUX
    if (!QFile::exists(chipBase())) {
        qDebug() << "[PWM] 未找到" << chipBase() << "，呼吸灯不可用（内核无 PWM 驱动？）";
        ready_ = false;
    } else if (!exportChannel()) {
        qDebug() << "[PWM] 导出通道失败:" << chanBase();
        ready_ = false;
    } else {
        writeAttr(chanBase() + "/period", QString::number(periodNs_));
        setDuty(0);
        setEnable(true);          /* 使能但占空比0 → 灯灭，待告警再呼吸 */
        ready_ = true;
        qDebug() << "[PWM] 就绪:" << chanBase();
    }
#else
    ready_ = false;               /* 非 Linux：空转，不影响编译运行 */
#endif
    timer_ = new QTimer(this);    /* 归属工作线程 */
    connect(timer_, &QTimer::timeout, this, &taskpwm::breatheStep);
}

/* 告警位掩码：!=0 表示有任意项超标 → 呼吸；==0 → 熄灭。只在状态翻转时动作。*/
void taskpwm::Slot_Alarm(unsigned int error_index)
{
    const bool on = (error_index != 0);
    if (on == alarming_) return;
    alarming_ = on;

    if (on) {
        phase_ = 0.0;
        if (timer_) timer_->start(PWM_BREATH_MS);
    } else {
        if (timer_) timer_->stop();
        setDuty(0);               /* 熄灭 */
    }
}

/* 正弦渐变呼吸：level 在 0→1→0 之间平滑变化，映射到占空比。*/
void taskpwm::breatheStep()
{
    if (!ready_) return;
    phase_ += 0.08;
    if (phase_ > 2 * kPi) phase_ -= 2 * kPi;
    const double level = (1.0 - cos(phase_)) / 2.0;   /* 0..1 */
    setDuty((long)(level * periodNs_));
}

