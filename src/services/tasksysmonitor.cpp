#include "tasksysmonitor.h"
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QStringList>

tasksysmonitor::tasksysmonitor(QObject *parent)
    : QObject{parent}
{
}

/* 自动探测 CPU 温度文件：扫 /sys/class/thermal/thermal_zone*，命中 type 含 cpu
 * 的用其 temp；否则回退第一个 zone。与守护进程 resolve_cpu_temp_path 同逻辑。*/
void tasksysmonitor::resolveCpuTempPath()
{
    cpuTempPath_.clear();
#ifdef Q_OS_LINUX
    QDir base("/sys/class/thermal");
    const QStringList zones =
        base.entryList(QStringList() << "thermal_zone*", QDir::Dirs);
    QString fallback;
    for (const QString &z : zones) {
        const QString typePath = "/sys/class/thermal/" + z + "/type";
        const QString tempPath = "/sys/class/thermal/" + z + "/temp";
        if (fallback.isEmpty()) fallback = tempPath;
        QFile f(typePath);
        if (f.open(QIODevice::ReadOnly)) {
            const QString type = QString::fromUtf8(f.readAll()).trimmed().toLower();
            f.close();
            if (type.contains("cpu")) { cpuTempPath_ = tempPath; return; }
        }
    }
    cpuTempPath_ = fallback;
#endif
}

void tasksysmonitor::Start()
{
    resolveCpuTempPath();
    timer_ = new QTimer(this);      /* 在本(工作)线程创建，归属正确 */
    connect(timer_, &QTimer::timeout, this, &tasksysmonitor::sample);
    timer_->start(2000);            /* 2s 采一次 */
    sample();                       /* 立即来一发，界面不空等 */
}

void tasksysmonitor::sample()
{
    double cpuC = -1, availMB = -1, totalMB = -1;
#ifdef Q_OS_LINUX
    /* CPU 温度（毫摄氏度 → ℃）*/
    if (!cpuTempPath_.isEmpty()) {
        QFile f(cpuTempPath_);
        if (f.open(QIODevice::ReadOnly)) {
            bool ok = false;
            long milli = QString::fromUtf8(f.readAll()).trimmed().toLong(&ok);
            f.close();
            if (ok) cpuC = milli / 1000.0;
        }
    }
    /* 内存：/proc/meminfo（kB）*/
    QFile m("/proc/meminfo");
    if (m.open(QIODevice::ReadOnly)) {
        long total = -1, avail = -1, freeKb = -1;
        while (!m.atEnd()) {
            const QByteArray line = m.readLine();
            if (total < 0 && line.startsWith("MemTotal:"))
                total = line.mid(9).trimmed().split(' ').first().toLong();
            else if (avail < 0 && line.startsWith("MemAvailable:"))
                avail = line.mid(13).trimmed().split(' ').first().toLong();
            else if (freeKb < 0 && line.startsWith("MemFree:"))
                freeKb = line.mid(8).trimmed().split(' ').first().toLong();
        }
        m.close();
        if (avail < 0) avail = freeKb;          /* 老内核无 MemAvailable 退回 MemFree */
        if (total > 0) totalMB = total / 1024.0;
        if (avail >= 0) availMB = avail / 1024.0;
    }
#endif
    emit Signal_SysHealth(cpuC, availMB, totalMB);
}

