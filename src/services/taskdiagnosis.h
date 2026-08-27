#ifndef TASKDIAGNOSIS_H
#define TASKDIAGNOSIS_H

#include <QObject>
#include <QString>

/* 边缘 AI —— 传感器故障诊断（规则+统计法，无需训练数据，板上轻量可跑）。
 * 每次串口解析完(Widget::Signal_SerialPortProcess)调用一次 TaskDiagnosis()，
 * 为每节点每指标维护滑动窗口，检测：卡死 / 越界 / 跳变。
 * 只对"在线且有数据"的节点诊断（离线由 widget 的时间戳机制负责）。 */
class taskdiagnosis : public QObject
{
    Q_OBJECT
public:
    explicit taskdiagnosis(QObject *parent = nullptr);

public slots:
    void TaskDiagnosis();   // 在诊断线程执行

signals:
    // node: 1/2/3；reason: 人类可读的诊断结论（仅在故障态变化时发，避免刷屏）
    void Signal_Diagnosis(unsigned int node, const QString &reason);
};

#endif // TASKDIAGNOSIS_H
