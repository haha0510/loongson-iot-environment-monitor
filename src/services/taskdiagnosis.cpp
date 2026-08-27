#include "taskdiagnosis.h"
#include "widget.h"     /* 为了读取全局 temp*/humi*/... 与 state_node* */
#include <math.h>

/* ---- 物理量程与跳变上限（可调）；index: 0=温度 1=湿度 2=光照 3=烟雾 ---- */
static const double kMin[4]     = { -40.0,   0.0,     0.0,    0.0 };
static const double kMax[4]     = { 125.0, 100.0, 65535.0, 2000.0 };
static const double kMaxStep[4] = {  20.0,  40.0,  5000.0, 1000.0 }; /* 相邻两拍最大合理变化 */
static const char  *kName[4]    = { "温度", "湿度", "光照", "烟雾" };

#define DIAG_WIN 10        /* 滑动窗口长度（次）*/

/* [节点0..2][指标0..3] 的滑动窗口与状态（单实例，用文件静态即可）*/
static double s_win[3][4][DIAG_WIN];
static int    s_cnt[3][4];
static int    s_head[3][4];
static int    s_lastFault[3][4];   /* 0正常 1卡死 2越界 3跳变 */

taskdiagnosis::taskdiagnosis(QObject *parent)
    : QObject{parent}
{
    for (int n = 0; n < 3; n++)
        for (int m = 0; m < 4; m++) {
            s_cnt[n][m] = 0; s_head[n][m] = 0; s_lastFault[n][m] = 0;
        }
}

/* 取节点 n(0..2) 当前四指标值；离线返回 false。*/
static bool nodeSample(int n, double v[4])
{
    switch (n) {
    case 0: if (!state_nodeOne) return false;
        v[0]=temp1; v[1]=humi1; v[2]=light1; v[3]=smog1; return true;
    case 1: if (!state_nodeTwo) return false;
        v[0]=temp2; v[1]=humi2; v[2]=light2; v[3]=smog2; return true;
    case 2: if (!state_nodeThr) return false;
        v[0]=temp3; v[1]=humi3; v[2]=light3; v[3]=smog3; return true;
    }
    return false;
}

void taskdiagnosis::TaskDiagnosis()
{
    for (int n = 0; n < 3; n++) {
        double v[4];
        if (!nodeSample(n, v)) {
            /* 离线：清窗口与故障态，避免上线后拿旧数据误判 */
            for (int m = 0; m < 4; m++) { s_cnt[n][m] = 0; s_lastFault[n][m] = 0; }
            continue;
        }

        for (int m = 0; m < 4; m++) {
            double cur = v[m];
            int fault = 0;                 /* 本拍判定 */
            const char *detail = "";

            /* 1) 越界：超物理量程 */
            if (cur < kMin[m] || cur > kMax[m]) {
                fault = 2; detail = "读数越界(超物理量程)";
            }
            /* 2) 跳变：与上一拍变化率超物理可能 */
            else if (s_cnt[n][m] > 0) {
                int prevIdx = (s_head[n][m] - 1 + DIAG_WIN) % DIAG_WIN;
                if (fabs(cur - s_win[n][m][prevIdx]) > kMaxStep[m]) {
                    fault = 3; detail = "读数跳变(疑毛刺/接触不良)";
                }
            }

            /* 入窗（环形缓冲）*/
            s_win[n][m][s_head[n][m]] = cur;
            s_head[n][m] = (s_head[n][m] + 1) % DIAG_WIN;
            if (s_cnt[n][m] < DIAG_WIN) s_cnt[n][m]++;

            /* 3) 卡死：窗口满且全窗数值完全不变 */
            if (fault == 0 && s_cnt[n][m] >= DIAG_WIN) {
                double mn = s_win[n][m][0], mx = s_win[n][m][0];
                for (int i = 1; i < DIAG_WIN; i++) {
                    if (s_win[n][m][i] < mn) mn = s_win[n][m][i];
                    if (s_win[n][m][i] > mx) mx = s_win[n][m][i];
                }
                if (mx - mn < 1e-9) { fault = 1; detail = "数值卡死(疑传感器卡死/掉采)"; }
            }

            /* 仅在故障态变化时上报，避免每拍刷屏 */
            if (fault != s_lastFault[n][m]) {
                s_lastFault[n][m] = fault;
                if (fault != 0)
                    emit Signal_Diagnosis((unsigned int)(n + 1),
                                          QString("%1传感器%2").arg(kName[m]).arg(detail));
            }
        }
    }
}

