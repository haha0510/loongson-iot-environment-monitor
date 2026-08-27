/* ============================================================================
 * healthon_monitor —— 龙芯 2K1000LA 版（由 X5 板 v3 移植，方案A：独立守护进程）
 * ============================================================================
 *
 * 【移植说明】本文件由 D-Robotics X5 的 healthon_monitor v3 移植而来，作为
 * **独立守护进程**与龙芯环境监测 GUI（Loong_disp）并排运行，代码零耦合。
 * 相对 X5 版的关键改动均标注 [龙芯适配]：
 *   1. CPU 温度不再硬编码 thermal_zone1，改为**运行时自动探测**含 "cpu" 的
 *      thermal_zone（2K1000LA 片内只有 1 个温度传感器，见手册第 8 章）。
 *   2. 删除 DDR 温度采集：2K1000LA 无独立 DDR 温度传感器（手册：SEL 只可为 0）。
 *   3. 持久化目录 /userdata → /var/log/healthon（龙芯 rootfs 无 /userdata）。
 *   4. 保护名单补入 GUI 进程 Loong_disp、Xorg。
 *
 * 【硬件背景（2K1000LA 用户手册 第8章 + ACPI 温控）】
 *   - 片内 1 个温度传感器：结点温度 = 采样值-100，量程 -40~125℃，>125℃ 溢出。
 *   - 芯片自带 3 级硬件温控：WARNING_TMP(建议降功耗) → ALERT_TMP(建议正常关机)
 *     → CTT 临界(无条件进入 G2/S5 硬断电)。本进程软件 reboot 阈值(100℃) 应
 *     低于硬件 CTT，作为更早、更温和的一层干预。
 *
 * 【处置策略】（沿用 v3）
 *   CPU >= 85℃            → [WARN] 只记日志
 *   CPU >= 100℃ 连续3次   → [FATAL] reboot（受风暴保护约束，须 < 硬件CTT）
 *   可用内存 < 100MB 连续2次 → [ACTION] 杀 RSS 最大的非保护进程
 *
 * 【用法】
 *   ./healthon_monitor           前台运行（调试，Ctrl+C 退出）
 *   ./healthon_monitor -d        守护进程模式（后台常驻）
 *   ./healthon_monitor -n        演练模式：只记录"本应执行的动作"，不真执行
 *   ./healthon_monitor -n -d     后台演练（上生产前先这样跑几天）
 *   HM_DRY_RUN=1 ...             环境变量方式开演练
 *   HM_TEMP_PATH=/sys/.../temp   手动指定温度文件，跳过自动探测
 *
 * 【上板前必做】
 *   1. ps -o pid,comm            查真实进程名，补全 protected_names[]
 *   2. for z in /sys/class/thermal/thermal_zone*; do \
 *          echo "$z $(cat $z/type)"; done    确认自动探测选中的温度区正确
 *   3. df /var/log               确认非 tmpfs（否则重启历史会丢，熔断失效）
 * ==========================================================================*/

/* 必须在任何系统头文件之前：暴露 fork/setsid/flock/usleep/dprintf 等
 * POSIX/BSD 接口。否则严格 -std=c11 下 glibc 会把它们隐藏，导致隐式声明。 */
#define _DEFAULT_SOURCE 1

#include <stdio.h>      /* fopen/fgets/fscanf/snprintf/printf */
#include <stdlib.h>     /* atoi/system/getenv */
#include <string.h>     /* strcmp/strncmp/strcspn/strstr/strerror */
#include <errno.h>      /* errno */
#include <signal.h>     /* signal/kill/sig_atomic_t */
#include <time.h>       /* time/localtime/strftime */
#include <unistd.h>     /* fork/setsid/chdir/dup2/sleep/getpid/read/sync */
#include <fcntl.h>      /* open 及 O_RDWR/O_CREAT */
#include <sys/file.h>   /* flock */
#include <sys/stat.h>   /* stat/mkdir/umask */
#include <dirent.h>     /* opendir/readdir */

/* ---------------------------------------------------------------------------
 * 路径配置  [龙芯适配] /userdata → /var/log
 * 全部绝对路径！daemonize 后工作目录切到 "/"，相对路径全失效。
 * /var/log 在龙芯 Loongnix/Debian rootfs 上是持久化分区（非 tmpfs）；若你的
 * rootfs 把 /var 挂成 tmpfs，务必改到真正掉电不丢的目录，否则重启历史一丢，
 * 风暴熔断保护就形同虚设。上板用 `df /var/log` 确认。
 * ------------------------------------------------------------------------ */
#define LOG_DIR          "/var/log/healthon"
#define LOG_PATH         LOG_DIR "/healthon_monitor.log"
#define PID_PATH         LOG_DIR "/healthon_monitor.pid"
#define REBOOT_HIST_PATH LOG_DIR "/reboot_history"
#define LOG_MAX_SIZE     (512 * 1024)   /* 单个日志上限 512KB，超则轮转 */
#define INTERVAL         5              /* 采样间隔（秒）*/

/* [龙芯适配] cpufreq：2K1000LA 支持 DVFS，若内核带 cpufreq 驱动即有此文件；
 * 没有也不影响，读失败显示 N/A。仅展示用，不参与决策。 */
#define PATH_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"
#define PATH_MEM  "/proc/meminfo"

/* ---------------------------------------------------------------------------
 * 健康动作阈值（连续确认 = 软件滤波，防 sysfs 单次读数毛刺误触发）
 * ------------------------------------------------------------------------ */
#define CPU_WARN_C        85     /* 告警线：只记 [WARN] */
#define CPU_CRIT_C        100    /* 重启线：达到开始计数（应 < 硬件 CTT 临界） */
#define CPU_CRIT_CONFIRM  3      /* 3 次 x 5 秒 = 持续 15 秒才真重启 */

#define MEM_LOW_KB        (100 * 1024)  /* 可用内存告警线：100MB（kB） */
#define MEM_LOW_CONFIRM   2             /* OOM 恶化快，确认次数少些 */

#define REBOOT_WINDOW_SEC 3600   /* 风暴保护统计窗口：1 小时 */
#define REBOOT_STORM_MAX  2      /* 窗口内最多自动重启 2 次，第 3 次锁死只报警 */

/* ---------------------------------------------------------------------------
 * 全局状态
 * ------------------------------------------------------------------------ */
static volatile sig_atomic_t running = 1;   /* 信号标志位（volatile+原子） */
static int pid_fd = -1;                      /* 锁文件 fd，全程不 close */
static int dry_run = 0;                      /* 演练模式开关 */

/* [龙芯适配] 运行时探测到的 CPU 温度文件路径，由 resolve_cpu_temp_path() 填充 */
static char g_cpu_temp_path[192] = "";

/* 信号处理：只设标志，收尾留给主循环 */
static void handle_signal(int sig)
{
    (void)sig;
    running = 0;
}

/* ============================================================================
 * 采集层：只读数据，绝不动手
 * ==========================================================================*/

/* 从"内容是一个数字"的文件读出该数；失败返回 -1（调用方显示 N/A 并跳过判定）。*/
static long read_long_file(const char *path)
{
    if (!path || !path[0])
        return -1;
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;
    long val;
    if (fscanf(fp, "%ld", &val) != 1)
        val = -1;
    fclose(fp);
    return val;
}

/* [龙芯适配] 运行时探测 CPU 温度文件路径。
 * 扫描 /sys/class/thermal/thermal_zone*，读各 zone 的 type，命中含 "cpu" 的即
 * 用其 temp 文件；找不到则回退到第一个 zone。2K1000LA 片内只有 1 个传感器，
 * 内核 thermal 驱动已把结点温度换算成毫摄氏度（含 -100 偏移），故沿用
 * read_long_file + /1000.0 即可，无需直接读 0x1fe01510 寄存器。
 * 结果写 g_cpu_temp_path；返回 0 成功、-1 一个 zone 都没有。 */
static int resolve_cpu_temp_path(void)
{
    const char *env = getenv("HM_TEMP_PATH");   /* 允许手动指定，跳过探测 */
    if (env && env[0]) {
        snprintf(g_cpu_temp_path, sizeof(g_cpu_temp_path), "%s", env);
        return 0;
    }

    DIR *d = opendir("/sys/class/thermal");
    if (!d)
        return -1;

    char fallback[192] = "";
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strncmp(ent->d_name, "thermal_zone", 12) != 0)
            continue;

        char type_path[256], type[64] = "";
        snprintf(type_path, sizeof(type_path),
                 "/sys/class/thermal/%s/type", ent->d_name);
        FILE *tf = fopen(type_path, "r");
        if (tf) {
            if (fgets(type, sizeof(type), tf))
                type[strcspn(type, "\n")] = '\0';
            fclose(tf);
        }
        if (fallback[0] == '\0')             /* 记住第一个 zone 作兜底 */
            snprintf(fallback, sizeof(fallback),
                     "/sys/class/thermal/%s/temp", ent->d_name);

        for (char *p = type; *p; p++)        /* 手写 tolower，免依赖 ctype */
            if (*p >= 'A' && *p <= 'Z') *p += 32;
        if (strstr(type, "cpu")) {           /* 龙芯常见 type 如 cpu-thermal */
            snprintf(g_cpu_temp_path, sizeof(g_cpu_temp_path),
                     "/sys/class/thermal/%s/temp", ent->d_name);
            closedir(d);
            return 0;
        }
    }
    closedir(d);

    if (fallback[0]) {
        snprintf(g_cpu_temp_path, sizeof(g_cpu_temp_path), "%s", fallback);
        return 0;
    }
    return -1;
}

/* 解析 /proc/meminfo，拿总内存与可用内存（kB）。*/
static int read_meminfo(long *total_kb, long *avail_kb)
{
    FILE *fp = fopen(PATH_MEM, "r");
    if (!fp)
        return -1;
    char line[256];
    long t = -1, f = -1, a = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (t < 0 && sscanf(line, "MemTotal: %ld kB", &t) == 1)
            continue;
        if (f < 0 && sscanf(line, "MemFree: %ld kB", &f) == 1)
            continue;
        if (a < 0 && sscanf(line, "MemAvailable: %ld kB", &a) == 1)
            continue;
    }
    fclose(fp);
    *total_kb = t;
    *avail_kb = (a >= 0) ? a : f;   /* 优先 MemAvailable，老内核退回 MemFree */
    return (t > 0 && *avail_kb >= 0) ? 0 : -1;
}

/* 毫摄氏度 → "84.1 C"；读失败(-1) → "N/A"。 */
static void fmt_temp(long milli, char *buf, size_t sz)
{
    if (milli >= 0)
        snprintf(buf, sz, "%.1f C", milli / 1000.0);
    else
        snprintf(buf, sz, "N/A");
}

/* 采集全部指标拼一行日志，并把决策层要用的原始数值经出参带出。
 * [龙芯适配] 去掉 DDR 温度，CPU 温度取自 g_cpu_temp_path。 */
static void build_line(char *out, size_t outsz, long *out_cpu_milli, long *out_avail_kb)
{
    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));

    long cpu_milli = read_long_file(g_cpu_temp_path);
    long freq_khz  = read_long_file(PATH_FREQ);   /* cpufreq 单位 kHz */

    long total_kb = -1, avail_kb = -1;
    int mem_ok = (read_meminfo(&total_kb, &avail_kb) == 0);

    char tc_buf[32], f_buf[32], m_buf[48];
    fmt_temp(cpu_milli, tc_buf, sizeof(tc_buf));

    if (freq_khz < 0)
        snprintf(f_buf, sizeof(f_buf), "N/A");
    else
        snprintf(f_buf, sizeof(f_buf), "%.1f MHz", freq_khz / 1000.0);

    if (mem_ok)
        snprintf(m_buf, sizeof(m_buf), "%.1f/%.1f MB",
                 avail_kb / 1024.0, total_kb / 1024.0);
    else
        snprintf(m_buf, sizeof(m_buf), "N/A");

    snprintf(out, outsz, "%s | CPU: %s | Freq: %s | Mem: %s",
             ts, tc_buf, f_buf, m_buf);

    *out_cpu_milli = cpu_milli;
    *out_avail_kb  = mem_ok ? avail_kb : -1;
}

/* ============================================================================
 * 底座：单实例锁（flock 生命周期 = fd 生命周期，进程死内核自动放锁）
 * ==========================================================================*/
static int acquire_lock(void)
{
    pid_fd = open(PID_PATH, O_RDWR | O_CREAT, 0644);
    if (pid_fd < 0) {
        fprintf(stderr, "open %s 失败: %s\n", PID_PATH, strerror(errno));
        return -1;
    }
    if (flock(pid_fd, LOCK_EX | LOCK_NB) < 0) {
        char buf[32] = {0};
        ssize_t n = read(pid_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[strcspn(buf, "\n")] = '\0';
            fprintf(stderr, "已有实例在运行 (pid %s)，本次退出\n", buf);
        } else {
            fprintf(stderr, "已有实例在运行，本次退出\n");
        }
        return -1;
    }
    return 0;   /* fork 后 pid 会变，此刻先不写 pid */
}

/* 把最终 pid 写进锁文件。必须在 daemonize 之后调。 */
static void write_pidfile(void)
{
    if (ftruncate(pid_fd, 0) < 0)
        return;
    lseek(pid_fd, 0, SEEK_SET);
    dprintf(pid_fd, "%d\n", getpid());
}

/* ============================================================================
 * 底座：守护进程化——经典 double-fork 七步
 * ==========================================================================*/
static int daemonize(void)
{
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid > 0) _exit(0);          /* 第1步：父退出，子非组长 */

    if (setsid() < 0) return -1;    /* 第2步：新会话，脱离控制终端 */

    pid = fork();
    if (pid < 0) return -1;
    if (pid > 0) _exit(0);          /* 第3步：二次 fork，永不再获控制终端 */

    umask(022);                     /* 第4步：重置权限掩码 */
    if (chdir("/") < 0) return -1;  /* 第5步：cwd 挪到根，不占用文件系统 */

    /* 第6步：三个标准流重定向到 /dev/null */
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0) return -1;
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > STDERR_FILENO) close(fd);
    return 0;
}

/* ============================================================================
 * 底座：日志轮转 + 统一写入口
 * ==========================================================================*/
static void rotate_if_needed(void)
{
    struct stat st;
    if (stat(LOG_PATH, &st) < 0 || st.st_size < LOG_MAX_SIZE)
        return;
    remove(LOG_PATH ".2");
    rename(LOG_PATH ".1", LOG_PATH ".2");
    rename(LOG_PATH, LOG_PATH ".1");
}

static void log_line(const char *line)
{
    rotate_if_needed();
    FILE *fp = fopen(LOG_PATH, "a");
    if (!fp)
        return;
    fprintf(fp, "%s\n", line);
    fclose(fp);
}

/* ============================================================================
 * 决策+动作：重启风暴保护（熔断）
 * 历史文件每行一个 Unix 时间戳，存持久化目录（不可放 tmpfs）。
 * ==========================================================================*/
static int should_allow_reboot_and_record(void)
{
    time_t now = time(NULL);
    time_t recent[64];
    int n = 0;

    FILE *fp = fopen(REBOOT_HIST_PATH, "r");
    if (fp) {
        long v;
        while (n < 64 && fscanf(fp, "%ld", &v) == 1) {
            if (now - (time_t)v < REBOOT_WINDOW_SEC)
                recent[n++] = (time_t)v;   /* 只留窗口内记录 */
        }
        fclose(fp);
    }

    if (n >= REBOOT_STORM_MAX)
        return 0;                          /* 熔断：窗口内次数已满 */

    fp = fopen(REBOOT_HIST_PATH, "w");     /* 放行：旧记录+本次 重写回 */
    if (fp) {
        for (int i = 0; i < n; i++)
            fprintf(fp, "%ld\n", (long)recent[i]);
        fprintf(fp, "%ld\n", (long)now);
        fclose(fp);
    }
    return 1;
}

/* 执行重启（或演练模式假装执行）。 */
static void do_reboot(const char *reason)
{
    char line[256];
    snprintf(line, sizeof(line), "[FATAL] 触发重启: %s", reason);
    log_line(line);
    sync();     /* 黑匣子：把日志页缓存强刷到存储，防重启丢证据 */

    if (dry_run) {
        log_line("[DRY-RUN] 演练模式，已跳过真实重启");
        return;
    }
    int rc = system("reboot");
    if (rc != 0) {
        snprintf(line, sizeof(line), "[ERROR] reboot 命令返回异常 (%d): %s",
                 rc, strerror(errno));
        log_line(line);
    }
}

/* ============================================================================
 * 决策层：CPU 温度分级响应
 * ==========================================================================*/
static void check_thermal(long cpu_milli)
{
    static int crit_count = 0;   /* 函数私有的跨调用计数器 */
    char line[160];

    if (cpu_milli < 0)
        return;                  /* 读数无效：宁可漏判，绝不拿坏数据决策 */

    double c = cpu_milli / 1000.0;

    if (c >= CPU_CRIT_C) {
        crit_count++;
        snprintf(line, sizeof(line), "[CRIT] CPU %.1f C >= %d C，连续 %d/%d 次确认",
                 c, CPU_CRIT_C, crit_count, CPU_CRIT_CONFIRM);
        log_line(line);

        if (crit_count >= CPU_CRIT_CONFIRM) {
            if (should_allow_reboot_and_record())
                do_reboot(line);
            else
                log_line("[LOCKDOWN] 1小时内已重启达上限，本次不再重启，"
                          "请人工检查过温根因（散热/负载）");
            crit_count = 0;      /* 动没动手都清零，给系统观察窗口 */
        }
    } else {
        crit_count = 0;          /* 回落即清零：语义是"连续N次"而非"累计N次" */
        if (c >= CPU_WARN_C) {
            snprintf(line, sizeof(line), "[WARN] CPU %.1f C >= %d C", c, CPU_WARN_C);
            log_line(line);
        }
    }
}

/* ============================================================================
 * 决策+动作：内存治理（找 RSS 最大户 → 先礼后兵杀掉）
 * ==========================================================================*/

/* 保护名单：就算 RSS 最大也绝不下手的进程。
 * [龙芯适配] 补入 GUI 进程 "Loong_disp" 与桌面 "Xorg"——Qt+Charts 界面很可能
 * 正是全机 RSS 最大进程，不保护它，低内存时这个守护进程会亲手杀掉被监控的界面!
 * /proc/[pid]/comm 最长 15 字符，长名会被内核截断，故用 strncmp 前缀匹配。
 * 上板前务必 `ps -o pid,comm` 核对真实进程名并补全。 */
static const char *protected_names[] = {
    "init", "systemd", "supervisord", "sshd", "dropbear",
    "syslogd", "klogd", "udevd", "healthon_monito",
    "Loong_disp", "Xorg",   /* [龙芯适配] GUI 与桌面，绝不可杀 */
    /* TODO(上板前补): 其它必须存活的产品/系统进程 */
};
#define PROTECTED_COUNT (sizeof(protected_names) / sizeof(protected_names[0]))

static int is_protected(int pid, const char *comm)
{
    if (pid <= 1)          return 1;   /* init：系统之根 */
    if (pid == getpid())   return 1;   /* 自己 */
    for (size_t i = 0; i < PROTECTED_COUNT; i++)
        if (strncmp(comm, protected_names[i], strlen(protected_names[i])) == 0)
            return 1;                  /* 前缀命中即保护 */
    return 0;
}

/* 读某进程 VmRSS（实际物理内存 kB）。内核线程无 VmRSS → 返回 -1 被自然排除。 */
static long read_rss_kb(int pid)
{
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;
    char line[256];
    long rss = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "VmRSS: %ld kB", &rss) == 1)
            break;
    }
    fclose(fp);
    return rss;
}

/* 遍历 /proc 找 RSS 最大的非保护进程；找不到返回 -1。 */
static int find_max_rss_process(long *out_rss_kb, char *out_comm, size_t comm_sz)
{
    DIR *d = opendir("/proc");
    if (!d)
        return -1;

    struct dirent *ent;
    int  best_pid = -1;
    long best_rss = -1;
    char best_comm[64] = "";

    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] < '0' || ent->d_name[0] > '9')
            continue;                    /* 跳过非数字条目 */
        int pid = atoi(ent->d_name);

        char comm_path[64], comm[64] = "";
        snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm", pid);
        FILE *cf = fopen(comm_path, "r");
        if (cf) {
            if (fgets(comm, sizeof(comm), cf))
                comm[strcspn(comm, "\n")] = '\0';
            fclose(cf);
        }

        if (is_protected(pid, comm))
            continue;

        long rss = read_rss_kb(pid);
        if (rss > best_rss) {             /* 擂台法打最大值 */
            best_rss = rss;
            best_pid = pid;
            snprintf(best_comm, sizeof(best_comm), "%s", comm);
        }
    }
    closedir(d);

    if (best_pid < 0)
        return -1;
    if (out_rss_kb) *out_rss_kb = best_rss;
    if (out_comm)   snprintf(out_comm, comm_sz, "%s", best_comm);
    return best_pid;
}

/* 终止目标进程：SIGTERM 先礼后兵，1 秒不走再 SIGKILL 强制。 */
static void terminate_process(int pid, const char *comm, long rss_kb)
{
    char line[192];

    if (dry_run) {
        snprintf(line, sizeof(line),
                 "[DRY-RUN] 本应终止 pid=%d(%s) RSS=%.1fMB，演练模式已跳过",
                 pid, comm, rss_kb / 1024.0);
        log_line(line);
        return;
    }

    snprintf(line, sizeof(line),
             "[ACTION] 内存告警触发，终止 pid=%d(%s) RSS=%.1fMB",
             pid, comm, rss_kb / 1024.0);
    log_line(line);

    kill(pid, SIGTERM);     /* 给机会体面退场 */

    for (int i = 0; i < 10; i++) {          /* 每 100ms 探一次，最多 1 秒 */
        usleep(100000);
        if (kill(pid, 0) < 0 && errno == ESRCH) {   /* 探活：进程已没 */
            log_line("[ACTION] 目标进程已退出（SIGTERM 生效）");
            return;
        }
    }

    kill(pid, SIGKILL);     /* 赖着不走：内核直接收尸 */
    log_line("[ACTION] SIGTERM 超时未生效，已发送 SIGKILL");
}

/* 决策层：内存低于阈值连续 MEM_LOW_CONFIRM 次 → 杀最大户。 */
static void check_memory(long avail_kb)
{
    static int low_count = 0;
    char line[160];

    if (avail_kb < 0)
        return;

    if (avail_kb < MEM_LOW_KB) {
        low_count++;
        snprintf(line, sizeof(line),
                 "[WARN] 可用内存 %.1f MB < %.0f MB，连续 %d/%d 次确认",
                 avail_kb / 1024.0, MEM_LOW_KB / 1024.0, low_count, MEM_LOW_CONFIRM);
        log_line(line);

        if (low_count >= MEM_LOW_CONFIRM) {
            long rss = -1;
            char comm[64];
            int pid = find_max_rss_process(&rss, comm, sizeof(comm));
            if (pid < 0)
                log_line("[WARN] 未找到可终止的候选进程（全部受保护或扫描失败）");
            else
                terminate_process(pid, comm, rss);
            low_count = 0;      /* 杀完清零，给回收/观察窗口，避免连环杀 */
        }
    } else {
        low_count = 0;
    }
}

/* ============================================================================
 * 主流程
 * 启动顺序：解析参数 → 建日志目录 → 探测温度区 → 抢锁(报错可见)
 *          → daemonize(变身) → 写pid(定型后)
 * ==========================================================================*/
int main(int argc, char **argv)
{
    int daemon_mode = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) daemon_mode = 1;
        if (strcmp(argv[i], "-n") == 0) dry_run = 1;
    }
    if (getenv("HM_DRY_RUN"))
        dry_run = 1;

    /* 日志目录：不存在就建；EEXIST 属正常。注意 EEXIST 不区分同名文件/目录。 */
    if (mkdir(LOG_DIR, 0755) < 0 && errno != EEXIST) {
        fprintf(stderr, "mkdir %s 失败: %s\n", LOG_DIR, strerror(errno));
        return 1;
    }

    /* [龙芯适配] 探测 CPU 温度文件（必须在 daemonize 之前，提示信息可见） */
    if (resolve_cpu_temp_path() < 0 || g_cpu_temp_path[0] == '\0') {
        fprintf(stderr, "警告: 未发现任何 thermal_zone，CPU 温度将显示 N/A，"
                        "过热保护失效。可用 HM_TEMP_PATH 手动指定。\n");
    } else {
        fprintf(stderr, "CPU 温度源: %s%s\n", g_cpu_temp_path,
                getenv("HM_TEMP_PATH") ? "（HM_TEMP_PATH 指定）" : "（自动探测）");
    }

    if (acquire_lock() < 0)
        return 1;

    if (daemon_mode) {
        printf("HealthOn Monitor 转入后台%s，日志: %s\n",
               dry_run ? "（演练模式）" : "", LOG_PATH);
        fflush(stdout);
        if (daemonize() < 0) {
            fprintf(stderr, "daemonize 失败: %s\n", strerror(errno));
            return 1;
        }
    }

    write_pidfile();    /* pid 已定型，现在写才是真身 */

    signal(SIGINT,  handle_signal);
    signal(SIGTERM, handle_signal);

    char line[256];
    unsigned long count = 0;

    snprintf(line, sizeof(line),
             "=== HealthOn Monitor 启动 (pid %d, %s模式%s, 温度源 %s) ===",
             getpid(), daemon_mode ? "后台" : "前台",
             dry_run ? "，演练 DRY-RUN" : "",
             g_cpu_temp_path[0] ? g_cpu_temp_path : "N/A");
    log_line(line);
    if (!daemon_mode)
        printf("%s\n", line);

    /* ---------------- 主循环：采集 → 记录 → 判定 → 睡 ---------------- */
    while (running) {
        long cpu_milli = -1, avail_kb = -1;
        build_line(line, sizeof(line), &cpu_milli, &avail_kb);
        log_line(line);
        if (!daemon_mode) {
            printf("%s\n", line);
            fflush(stdout);
        }

        check_thermal(cpu_milli);
        check_memory(avail_kb);

        count++;
        /* 睡眠拆成 1 秒粒度：收到退出信号后最多再等 1 秒 */
        for (int i = 0; i < INTERVAL && running; i++)
            sleep(1);
    }

    /* ---------------- 优雅退场：留遗言，刷落盘 ---------------- */
    snprintf(line, sizeof(line), "=== 退出 HealthOn Monitor，共采样 %lu 次 ===", count);
    log_line(line);
    sync();
    return 0;

}









