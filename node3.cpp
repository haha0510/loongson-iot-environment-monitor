#include "node3.h"
#include "ui_node3.h"

node3::node3(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::node3)
{
    ui->setupUi(this);
    Init_Chart();
}

node3::~node3()
{
    delete ui;
}
void node3::Init_Chart()
{
    chart_nodeThr = new QChart();       // - 初始化QChart的实例
    QPen pen(Qt::red);          // - 画笔对象
    pen.setWidth(3);
    // == 曲线1 ==
    Series_nodeThr_One = new QSplineSeries();        // - 初始化QSplineSeries的实例
    Series_nodeThr_One->setName("温度曲线");     // - 设置曲线的名称
    Series_nodeThr_One->setPen(pen);
    chart_nodeThr->addSeries(Series_nodeThr_One);     // - 把曲线添加到QChart的实例chart中
    // == 曲线2 ==
    Series_nodeThr_Two = new QSplineSeries();
    Series_nodeThr_Two->setName("湿度曲线");
    pen.setColor(Qt::blue);
    Series_nodeThr_Two->setPen(pen);
    chart_nodeThr->addSeries(Series_nodeThr_Two);// - 把曲线添加到QChart的实例chart中
    // == 曲线3 ==
    Series_nodeThr_Thr = new QSplineSeries();
    Series_nodeThr_Thr->setName("光照曲线");
    pen.setColor(Qt::green);
    Series_nodeThr_Thr->setPen(pen);
    chart_nodeThr->addSeries(Series_nodeThr_Thr);// - 把曲线添加到QChart的实例chart中
    // == 曲线4 ==
    Series_nodeThr_Fou = new QSplineSeries();
    Series_nodeThr_Fou->setName("烟雾曲线");
    pen.setColor(Qt::darkBlue);
    Series_nodeThr_Fou->setPen(pen);
    chart_nodeThr->addSeries(Series_nodeThr_Fou);// - 把曲线添加到QChart的实例chart中



    // == 坐标轴设置 ==
    axisX_nodeThr = new QDateTimeAxis();        // - 声明并初始化X轴、两个Y轴

    axisY_nodeThr = new QValueAxis();

    axisY_nodeThr->setRange(0, MAX_Y);       //温度范围
    axisX_nodeThr->setTickCount(10);            // - 设置坐标轴上的格点
    axisY_nodeThr->setTickCount(20);
    axisX_nodeThr->setFormat("hh:mm:ss");         //设置时间显示格式
    QFont font("Microsoft YaHei",8,QFont::Normal);      // - 微软雅黑。字体大小8
    axisX_nodeThr->setTitleFont(font);      // - 字体设置
    axisY_nodeThr->setTitleFont(font);
    axisX_nodeThr->setTitleText("时间轴");  // - 坐标轴名称设置
    axisY_nodeThr->setTitleText("温度值");
    //axisY->setGridLineVisible(false);       // - 设置网格不显示
    chart_nodeThr->addAxis(axisX_nodeThr, Qt::AlignBottom);     //下方：Qt::AlignBottom，左边：Qt::AlignLeft
    chart_nodeThr->addAxis(axisY_nodeThr, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_humi_nodeThr = new QValueAxis();
    axisY_humi_nodeThr->setRange(0, MAX_Y_HUMI); //湿度范围 0-99
    axisY_humi_nodeThr->setTickCount(20);
    axisY_humi_nodeThr->setTitleFont(font);
    axisY_humi_nodeThr->setTitleText("湿度值");
    chart_nodeThr->addAxis(axisY_humi_nodeThr, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_light_nodeThr = new QValueAxis();
    axisY_light_nodeThr->setRange(0, MAX_Y_LIGHT);//光照范围 0-499
    axisY_light_nodeThr->setTickCount(20);
    axisY_light_nodeThr->setTitleFont(font);
    axisY_light_nodeThr->setTitleText("光照值");
    chart_nodeThr->addAxis(axisY_light_nodeThr, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop


    // == 坐标轴设置 ==
    axisY_smog_nodeThr = new QValueAxis();
    axisY_smog_nodeThr->setRange(0, MAX_Y_SMOG);
    axisY_smog_nodeThr->setTickCount(20);
    axisY_smog_nodeThr->setTitleFont(font);
    axisY_smog_nodeThr->setTitleText("烟雾值");
    chart_nodeThr->addAxis(axisY_smog_nodeThr, Qt::AlignLeft);

    Series_nodeThr_One->attachAxis(axisX_nodeThr);       // - 把曲线关联到坐标轴
    Series_nodeThr_One->attachAxis(axisY_nodeThr);
    Series_nodeThr_Two->attachAxis(axisX_nodeThr);
    Series_nodeThr_Two->attachAxis(axisY_humi_nodeThr);
    Series_nodeThr_Thr->attachAxis(axisX_nodeThr);
    Series_nodeThr_Thr->attachAxis(axisY_light_nodeThr);
    Series_nodeThr_Fou->attachAxis(axisX_nodeThr);
    Series_nodeThr_Fou->attachAxis(axisY_smog_nodeThr);


    Series_nodeThr_One->show();
    Series_nodeThr_Two->hide();
    Series_nodeThr_Thr->hide();
    Series_nodeThr_Fou->hide();
    axisY_nodeThr->show();
    axisY_humi_nodeThr->hide();
    axisY_light_nodeThr->hide();
    axisY_smog_nodeThr->hide();
    // == 把chart显示到窗口上 ==
    ui->graphicsView->setChart(chart_nodeThr);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);// 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑
    chart_nodeThr->setTheme(QChart::ChartThemeBlueIcy);
}

bool mutex_log_3 = 0;
//unsigned char mutex_power1;
void node3::Slot_Warning(unsigned int error_index)
{
    if(state_nodeThr)//如果节点3上线
    {
        if(mutex_log_3 == 0)//如果之前是没上线的
        {
            mutex_log_3 = 1;
            ui->textEdit_log->setTextColor(Qt::blue);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点三已上线！");
            node_number++;
            node_disnumber--;
        }
        if(error_index & 0x40)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 温度异常！");
            ui->textEdit_temp->setTextColor(Qt::red);  //温度字体变红


            if(flag3==0)
            {
                flag3=1;
                emit Node3_Danger();

            }

        }
        else
        {
            ui->textEdit_temp->setTextColor(Qt::black);//字体黑
            error7 = 0;
            if(flag3==1)
            {
                flag3=0;
                emit Node3_Safe();

            }
        }


        if(error_index & 0x80)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 湿度异常！");
            ui->textEdit_humi->setTextColor(Qt::red);

            if(flag3==0)
            {
                flag3=1;
                emit Node3_Danger();

            }
        }
        else
        {
            ui->textEdit_humi->setTextColor(Qt::black);
            error8 = 0;
            if(flag3==1)
            {
                flag3=0;
                emit Node3_Safe();

            }
        }
        if(error_index & 0x100)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 光照异常！");
            ui->textEdit_light->setTextColor(Qt::red);

            if(flag3==0)
            {
                flag3=1;
                emit Node3_Danger();

            }
        }
        else
        {
            ui->textEdit_light->setTextColor(Qt::black);
            error9 = 0;
            if(flag3==1)
            {
                flag3=0;
                emit Node3_Safe();

            }
        }


        if(error_index & 0x4000)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ":烟雾颗粒超标！");
            ui->textEdit_smog->setTextColor(Qt::red);

            if(flag3==0)
            {
                flag3=1;
                emit Node3_Danger();

            }
        }
        else
        {
            ui->textEdit_smog->setTextColor(Qt::black);
            error15 = 0;
            if(flag3==1)
            {
                flag3=0;
                emit Node3_Safe();

            }
        }

        if((error_index & 0x41C0) == 0)
        {

            if(flag3==0)
            {
                flag3=1;
                emit Node3_Danger();

            }
        }

        ui->textEdit_temp->append(stringCurrentTimeMessage + ": " + stringTemp3 + "℃");
        ui->textEdit_humi->append(stringCurrentTimeMessage + ": " + stringHumi3 + "%RH");
        ui->textEdit_light->append(stringCurrentTimeMessage + ": " + stringLight3 + "lx");
        ui->textEdit_smog->append(stringCurrentTimeMessage + ": " + stringSmog3 + "mg");

    }


}
void node3::downline()
{
    if(!state_nodeThr)
    {
        if(mutex_log_3)//如果之前是上线的
        {
            mutex_log_3 = 0;
            ui->textEdit_log->setTextColor(Qt::black);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点三已下线！");

            node_number--;
            node_disnumber++;
            error7 = error8 = error9 = error12 =error15 = 0;
        }
    }

}
void node3::on_temperature3Button_clicked()
{
    Series_nodeThr_One->show();
    Series_nodeThr_Two->hide();
    Series_nodeThr_Thr->hide();
    axisY_nodeThr->show();
    axisY_humi_nodeThr->hide();
    axisY_light_nodeThr->hide();
    Series_nodeThr_Fou->hide();
    axisY_smog_nodeThr->hide();
}


void node3::on_humi_3Button_clicked()
{
    Series_nodeThr_One->hide();
    Series_nodeThr_Two->show();
    Series_nodeThr_Thr->hide();
    axisY_nodeThr->hide();
    axisY_humi_nodeThr->show();
    axisY_light_nodeThr->hide();
    Series_nodeThr_Fou->hide();
    axisY_smog_nodeThr->hide();
}


void node3::on_light_3Button_clicked()
{
    Series_nodeThr_One->hide();
    Series_nodeThr_Two->hide();
    Series_nodeThr_Thr->show();
    axisY_nodeThr->hide();
    axisY_humi_nodeThr->hide();
    axisY_light_nodeThr->show();
    Series_nodeThr_Fou->hide();
    axisY_smog_nodeThr->hide();
}


void node3::on_smog_3Button_clicked()
{
    Series_nodeThr_One->hide();
    Series_nodeThr_Two->hide();
    Series_nodeThr_Thr->hide();
    axisY_nodeThr->hide();
    axisY_humi_nodeThr->hide();
    axisY_light_nodeThr->hide();
    Series_nodeThr_Fou->show();
    axisY_smog_nodeThr->show();
}


void node3::on_pushButton_data_clicked()
{
    if (ui->textEdit_temp->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "提示", "数据内容空");
        return;
    }
    QString data;
    data.append("节点三温度值");
    data.append("\r\n");
    data.append(ui->textEdit_temp->toPlainText());
    data.append("\r\n");
    data.append("节点三湿度值");
    data.append("\r\n");
    data.append(ui->textEdit_humi->toPlainText());
    data.append("\r\n");
    data.append("节点三光照值");
    data.append("\r\n");
    data.append(ui->textEdit_light->toPlainText());
    data.append("\r\n");
    data.append("节点三烟雾值");
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


void node3::on_back_3Button_clicked()
{
    emit Signal_Return();
}

void node3::Slot_RefreshTime()
{
    ui->label_time->setText(stringCurrentTime);
}
