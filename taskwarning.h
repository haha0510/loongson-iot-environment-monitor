#ifndef TASKWARNING_H
#define TASKWARNING_H

#include <QObject>

#include "widget.h"

class taskwarning : public QObject
{
    Q_OBJECT
public:
    explicit taskwarning(QObject *parent = nullptr);

     void TaskWarning();//TaskWarning----

signals:
     void Signal_Warning(unsigned int error_index);

};

#endif // TASKWARNING_H
