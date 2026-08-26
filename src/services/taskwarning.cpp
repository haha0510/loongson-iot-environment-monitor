#include "taskwarning.h"

taskwarning::taskwarning(QObject *parent)
    : QObject{parent}
{

}

void taskwarning::TaskWarning()
{

    unsigned int error_index = 0x0000;

    /* 节点状态检测现在主要由widget.cpp中的时间戳机制负责 */
    /* 这里只做简单的数据有效性检查作为辅助 */
    if(nodeNumber == 1)
    {
        // 如果接收到有效数据，确保节点状态为在线（时间戳检测会处理超时情况）
        if((temp1 != 0) || (humi1 != 0) || (light1 != 0) || (smog1 != 0))
        {
            state_nodeOne = 1;
        }
    }
    else if(nodeNumber == 2)
    {
        // 如果接收到有效数据，确保节点状态为在线（时间戳检测会处理超时情况）
        if((temp2 != 0) || (humi2 != 0) || (light2 != 0) || (smog2 != 0))
        {
            state_nodeTwo = 1;
        }
    }
    else if(nodeNumber == 3)
    {
        // 如果接收到有效数据，确保节点状态为在线（时间戳检测会处理超时情况）
        if((temp3 != 0) || (humi3 != 0) || (light3 != 0) || (smog3 != 0))
        {
            state_nodeThr = 1;
        }
    }

    /* Task of Warning */
    if(state_nodeOne)//如果节点1上线
    {
        /* Max && Min */
        if((temp1 > temp_max) || (temp1 < temp_min))
        {
            error_index |= 0x01; //节点1的温度异常
            error1 = 1;//节点1的温度异常
        }

        if((humi1 > humi_max) || (humi1 < humi_min))
        {
            error_index |= 0x02;
            error2 = 1;//节点1的湿度异常
        }

        if((light1 > light_max) || (light1 < light_min))
        {
            error_index |= 0x04;
            error3 = 1;//节点1光照异常
        }

        if((smog1 > smog_max))
        {
            error_index |= 0x1000;
            error13 = 1;
        }

    }
    if(state_nodeTwo)
    {
        /* Max && Min */
        if((temp2 > temp_max) || (temp2 < temp_min))
        {
            error_index |= 0x08;
            error4 = 1;
        }

        if((humi2 > humi_max) || (humi2 < humi_min))
        {
            error_index |= 0x10;
            error5 = 1;
        }

        if((light2 > light_max) || (light2 < light_min))
        {
            error_index |= 0x20;
            error6 = 1;
        }

        if((smog2 > smog_max))
        {
            error_index |= 0x2000;
            error14 = 1;
        }


    }
    if(state_nodeThr)
    {
        /* Max && Min */
        if((temp3 > temp_max) || (temp3 < temp_min))
        {
            error_index |= 0x40;
            error7 = 1;
        }

        if((humi3 > humi_max) || (humi3 < humi_min))
        {
            error_index |= 0x80;
            error8 = 1;
        }

        if((light3 > light_max) || (light3 < light_min))
        {
            error_index |= 0x100;
            error9 = 1;
        }

        if((smog3 > smog_max))
        {
            error_index |= 0x4000;
            error15 = 1;
        }

    }
    emit Signal_Warning(error_index);
}
