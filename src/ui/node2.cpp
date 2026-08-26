#include "node2.h"
#include "ui_node2.h"

node2::node2(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::node2)
{
    ui->setupUi(this);
    Init_Chart();
}

node2::~node2()
{
    delete ui;
}

void node2::Init_Chart()
{
    chart_nodeTwo = new QChart();       // - 初始化QChart的实例
    QPen pen(Qt::red);          // - 画笔对象
    pen.setWidth(3);
    // == 曲线1 ==
    Series_nodeTwo_One = new QSplineSeries();        // - 初始化QSplineSeries的实例
    Series_nodeTwo_One->setName("温度曲线");     // - 设置曲线的名称
    Series_nodeTwo_One->setPen(pen);
    chart_nodeTwo->addSeries(Series_nodeTwo_One);     // - 把曲线添加到QChart的实例chart中
    // == 曲线2 ==
    Series_nodeTwo_Two = new QSplineSeries();
    Series_nodeTwo_Two->setName("湿度曲线");
    pen.setColor(Qt::blue);
    Series_nodeTwo_Two->setPen(pen);
    chart_nodeTwo->addSeries(Series_nodeTwo_Two);// - 把曲线添加到QChart的实例chart中
    // == 曲线3 ==
    Series_nodeTwo_Thr = new QSplineSeries();
    Series_nodeTwo_Thr->setName("光照曲线");
    pen.setColor(Qt::green);
    Series_nodeTwo_Thr->setPen(pen);
    chart_nodeTwo->addSeries(Series_nodeTwo_Thr);// - 把曲线添加到QChart的实例chart中
    // == 曲线4 ==
    Series_nodeTwo_Fou = new QSplineSeries();
    Series_nodeTwo_Fou->setName("烟雾曲线");
    pen.setColor(Qt::darkBlue);
    Series_nodeTwo_Fou->setPen(pen);
    chart_nodeTwo->addSeries(Series_nodeTwo_Fou);// - 把曲线添加到QChart的实例chart中



    // == 坐标轴设置 ==
    axisX_nodeTwo = new QDateTimeAxis();        // - 声明并初始化X轴、两个Y轴
    //    axisX_nodeTwo = new QValueAxis();
    axisY_nodeTwo = new QValueAxis();
    //    axisX_nodeTwo->setRange(0, MAX_X);      // - 设置坐标轴显示的范围
    axisY_nodeTwo->setRange(0, MAX_Y);       //温度范围 0-39
    axisX_nodeTwo->setTickCount(10);            // - 设置坐标轴上的格点
    axisY_nodeTwo->setTickCount(20);
    axisX_nodeTwo->setFormat("hh:mm:ss");         //设置时间显示格式
    QFont font("Microsoft YaHei",8,QFont::Normal);      // - 微软雅黑。字体大小8
    axisX_nodeTwo->setTitleFont(font);      // - 字体设置
    axisY_nodeTwo->setTitleFont(font);
    axisX_nodeTwo->setTitleText("时间轴");  // - 坐标轴名称设置
    axisY_nodeTwo->setTitleText("温度值");
    //axisY->setGridLineVisible(false);       // - 设置网格不显示
    chart_nodeTwo->addAxis(axisX_nodeTwo, Qt::AlignBottom);     //下方：Qt::AlignBottom，左边：Qt::AlignLeft
    chart_nodeTwo->addAxis(axisY_nodeTwo, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_humi_nodeTwo = new QValueAxis();
    axisY_humi_nodeTwo->setRange(0, MAX_Y_HUMI); //湿度范围 0-99
    axisY_humi_nodeTwo->setTickCount(20);
    axisY_humi_nodeTwo->setTitleFont(font);
    axisY_humi_nodeTwo->setTitleText("湿度值");
    chart_nodeTwo->addAxis(axisY_humi_nodeTwo, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_light_nodeTwo = new QValueAxis();
    axisY_light_nodeTwo->setRange(0, MAX_Y_LIGHT);//光照范围 0-499
    axisY_light_nodeTwo->setTickCount(20);
    axisY_light_nodeTwo->setTitleFont(font);
    axisY_light_nodeTwo->setTitleText("光照值");
    chart_nodeTwo->addAxis(axisY_light_nodeTwo, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop


    // == 坐标轴设置 ==
    axisY_smog_nodeTwo = new QValueAxis();
    axisY_smog_nodeTwo->setRange(0, MAX_Y_SMOG);
    axisY_smog_nodeTwo->setTickCount(20);
    axisY_smog_nodeTwo->setTitleFont(font);
    axisY_smog_nodeTwo->setTitleText("烟雾值");
    chart_nodeTwo->addAxis(axisY_smog_nodeTwo, Qt::AlignLeft);

    Series_nodeTwo_One->attachAxis(axisX_nodeTwo);       // - 把曲线关联到坐标轴
    Series_nodeTwo_One->attachAxis(axisY_nodeTwo);
    Series_nodeTwo_Two->attachAxis(axisX_nodeTwo);
    Series_nodeTwo_Two->attachAxis(axisY_humi_nodeTwo);
    Series_nodeTwo_Thr->attachAxis(axisX_nodeTwo);
    Series_nodeTwo_Thr->attachAxis(axisY_light_nodeTwo);
    Series_nodeTwo_Fou->attachAxis(axisX_nodeTwo);
    Series_nodeTwo_Fou->attachAxis(axisY_smog_nodeTwo);


    Series_nodeTwo_One->show();
    Series_nodeTwo_Two->hide();
    Series_nodeTwo_Thr->hide();
    Series_nodeTwo_Fou->hide();
    axisY_nodeTwo->show();
    axisY_humi_nodeTwo->hide();
    axisY_light_nodeTwo->hide();
    axisY_smog_nodeTwo->hide();
    // == 把chart显示到窗口上 ==
    ui->graphicsView->setChart(chart_nodeTwo);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);// 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑
    chart_nodeTwo->setTheme(QChart::ChartThemeBlueIcy);
}

bool mutex_log_2 = 0;

void node2::Slot_Warning(unsigned int error_index)
{
    if(state_nodeTwo)//如果节点2上线
    {
        if(mutex_log_2 == 0)//如果之前是没上线的
        {
            mutex_log_2 = 1;
            ui->textEdit_log->setTextColor(Qt::blue);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点二已上线！");
            node_number++;
            node_disnumber--;
        }
        if(error_index & 0x08)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 温度异常！");
            ui->textEdit_temp->setTextColor(Qt::red);  //温度字体变红

            if(flag2==0)//flag2 控制只发一次警告信号
            {
                flag2=1;
                emit Node2_Danger();

            }

        }
        else
        {
            ui->textEdit_temp->setTextColor(Qt::black);//字体黑
            error4 = 0;
            if(flag2==1)
            {
                flag2=0;
                emit Node2_Safe();

            }
        }


        if(error_index & 0x10)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 湿度异常！");
            ui->textEdit_humi->setTextColor(Qt::red);

            if(flag2==0)
            {
                flag2=1;
                emit Node2_Danger();

            }
        }
        else
        {
            ui->textEdit_humi->setTextColor(Qt::black);
            error5 = 0;
            if(flag2==1)
            {
                flag2=0;
                emit Node2_Safe();

            }
        }
        if(error_index & 0x20)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 光照异常！");
            ui->textEdit_light->setTextColor(Qt::red);

            if(flag2==0)
            {
                flag2=1;
                emit Node2_Danger();

            }
        }
        else
        {
            ui->textEdit_light->setTextColor(Qt::black);
            error6 = 0;
            if(flag2==1)
            {
                flag2=0;
                emit Node2_Safe();

            }
        }


        if(error_index & 0x2000)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ":烟雾颗粒超标！");
            ui->textEdit_smog->setTextColor(Qt::red);

            if(flag2==0)
            {
                flag2=1;
                emit Node2_Danger();

            }
        }
        else
        {
            ui->textEdit_smog->setTextColor(Qt::black);
            error14 = 0;
            if(flag2==1)
            {
                flag2=0;
                emit Node2_Safe();

            }
        }

        if((error_index & 0x2038) == 0)
        {

            if(flag2==0)
            {
                flag2=1;
                emit Node2_Danger();

            }
        }

        ui->textEdit_temp->append(stringCurrentTimeMessage + ": " + stringTemp2 + "℃");
        ui->textEdit_humi->append(stringCurrentTimeMessage + ": " + stringHumi2 + "%RH");
        ui->textEdit_light->append(stringCurrentTimeMessage + ": " + stringLight2 + "lx");
        ui->textEdit_smog->append(stringCurrentTimeMessage + ": " + stringSmog2 + "mg");

    }

}
void node2::downline()
{
    if(!state_nodeTwo)
    {
        if(mutex_log_2)//如果之前是上线的
        {
            mutex_log_2 = 0;
            ui->textEdit_log->setTextColor(Qt::black);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点二已下线！");

            node_number--;
            node_disnumber++;
            error4 = error5 = error6 = error11 =error14 = 0;
        }
    }

}

void node2::on_temperature2Button_clicked()
{
    Series_nodeTwo_One->show();
    Series_nodeTwo_Two->hide();
    Series_nodeTwo_Thr->hide();
    axisY_nodeTwo->show();
    axisY_humi_nodeTwo->hide();
    axisY_light_nodeTwo->hide();
    Series_nodeTwo_Fou->hide();
    axisY_smog_nodeTwo->hide();
}


void node2::on_humi_2Button_clicked()
{
    Series_nodeTwo_One->hide();
    Series_nodeTwo_Two->show();
    Series_nodeTwo_Thr->hide();
    axisY_nodeTwo->hide();
    axisY_humi_nodeTwo->show();
    axisY_light_nodeTwo->hide();
    Series_nodeTwo_Fou->hide();
    axisY_smog_nodeTwo->hide();
}


void node2::on_light_2Button_clicked()
{
    Series_nodeTwo_One->hide();
    Series_nodeTwo_Two->hide();
    Series_nodeTwo_Thr->show();
    axisY_nodeTwo->hide();
    axisY_humi_nodeTwo->hide();
    axisY_light_nodeTwo->show();
    Series_nodeTwo_Fou->hide();
    axisY_smog_nodeTwo->hide();
}


void node2::on_smog_2Button_clicked()
{
    Series_nodeTwo_One->hide();
    Series_nodeTwo_Two->hide();
    Series_nodeTwo_Thr->hide();
    axisY_nodeTwo->hide();
    axisY_humi_nodeTwo->hide();
    axisY_light_nodeTwo->hide();
    Series_nodeTwo_Fou->show();
    axisY_smog_nodeTwo->show();
}


void node2::on_pushButton_data_clicked()
{
    if (ui->textEdit_temp->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "提示", "数据内容空");
        return;
    }
    QString data;
    data.append("节点二温度值");
    data.append("\r\n");
    data.append(ui->textEdit_temp->toPlainText());
    data.append("\r\n");
    data.append("节点二湿度值");
    data.append("\r\n");
    data.append(ui->textEdit_humi->toPlainText());
    data.append("\r\n");
    data.append("节点二光照值");
    data.append("\r\n");
    data.append(ui->textEdit_light->toPlainText());
    data.append("\r\n");
    data.append("节点二烟雾值");
    data.append("\r\n");
    data.append(ui->textEdit_smog->toPlainText());
    data.append("\r\n");
    QString curPath = QDir::currentPath();            //获取系统当前目录
    QString dlgTitle = "保存文件";                     //对话框标题
    QString filter = "文本文件(*.txt);;所有文件(*.*)";  //文件过滤器
    QString filename = QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (filename.isEmpty())
    {
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    /*保存文件*/
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << data;
    file.close();
}


void node2::on_back_2Button_clicked()
{
     emit Signal_Return();
}
void node2::Slot_RefreshTime()
{
    ui->label_time->setText(stringCurrentTime);
}
