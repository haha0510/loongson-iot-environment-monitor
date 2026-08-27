#ifndef TASKSYSMONITOR_H
#define TASKSYSMONITOR_H

#include <QObject>
#include <QString>

class QTimer;

/* 主机健康采集：定时读 sysfs 的 CPU 温度 + /proc/meminfo，发信号给 GUI。
 * CPU 温度自动探测（扫 /sys/class/thermal 命中 cpu 的 zone），逻辑与移植的
 * healthon 守护进程一致。仅 Linux(龙芯板)有真实读数，其它平台回 N/A。 */
class tasksysmonitor : public QObject
{
    Q_OBJECT
public:
    explicit tasksysmonitor(QObject *parent = nullptr);

public slots:
    void Start();      // 线程启动后调用：建定时器并开始采样

signals:
    // cpuC: CPU 温度℃(负=N/A); availMB/totalMB: 内存 MB(负=N/A)
    void Signal_SysHealth(double cpuC, double availMB, double totalMB);

private slots:
    void sample();

private:
    QTimer *timer_ = nullptr;
    QString cpuTempPath_;      // 自动探测到的温度文件路径
    void resolveCpuTempPath();
};

#endif // TASKSYSMONITOR_H
