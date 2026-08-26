#include "node1.h"
#include "ui_node1.h"

node1::node1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::node1)
{
    ui->setupUi(this);
    Init_Chart();//在node1类构建时就初始化图表
}

node1::~node1()
{
    delete ui;
}

void node1::Init_Chart()
{
    chart_nodeOne = new QChart();       // - 初始化QChart的实例
    QPen pen(Qt::red);          // - 画笔对象
    pen.setWidth(3);
    // == 曲线1 ==
    Series_nodeOne_One = new QSplineSeries();        // - 初始化QSplineSeries的实例
    Series_nodeOne_One->setName("温度曲线");     // - 设置曲线的名称
    Series_nodeOne_One->setPen(pen);
    chart_nodeOne->addSeries(Series_nodeOne_One);     // - 把曲线添加到QChart的实例chart中
    // == 曲线2 ==
    Series_nodeOne_Two = new QSplineSeries();
    Series_nodeOne_Two->setName("湿度曲线");
    pen.setColor(Qt::blue);
    Series_nodeOne_Two->setPen(pen);
    chart_nodeOne->addSeries(Series_nodeOne_Two);// - 把曲线添加到QChart的实例chart中
    // == 曲线3 ==
    Series_nodeOne_Thr = new QSplineSeries();
    Series_nodeOne_Thr->setName("光照曲线");
    pen.setColor(Qt::green);
    Series_nodeOne_Thr->setPen(pen);
    chart_nodeOne->addSeries(Series_nodeOne_Thr);// - 把曲线添加到QChart的实例chart中
    // == 曲线4 ==
    Series_nodeOne_Fou = new QSplineSeries();
    Series_nodeOne_Fou->setName("烟雾曲线");
    pen.setColor(Qt::darkBlue);
    Series_nodeOne_Fou->setPen(pen);
    chart_nodeOne->addSeries(Series_nodeOne_Fou);// - 把曲线添加到QChart的实例chart中



    // == 坐标轴设置 ==
    axisX_nodeOne = new QDateTimeAxis();        // - 声明并初始化X轴、两个Y轴
    //    axisX_nodeOne = new QValueAxis();
    axisY_nodeOne_temp = new QValueAxis();
    //    axisX_nodeOne->setRange(0, MAX_X);      // - 设置坐标轴显示的范围
    axisY_nodeOne_temp->setRange(0, MAX_Y);       //温度范围 0-39
    axisX_nodeOne->setTickCount(10);            // - 设置坐标轴上的格点
    axisY_nodeOne_temp->setTickCount(20);
    axisX_nodeOne->setFormat("hh:mm:ss");         //设置时间显示格式
    QFont font("Microsoft YaHei",8,QFont::Normal);      // - 微软雅黑。字体大小8
    axisX_nodeOne->setTitleFont(font);      // - 字体设置
    axisY_nodeOne_temp->setTitleFont(font);
    axisX_nodeOne->setTitleText("时间轴");  // - 坐标轴名称设置
    axisY_nodeOne_temp->setTitleText("温度值");
    //axisY->setGridLineVisible(false);       // - 设置网格不显示
    chart_nodeOne->addAxis(axisX_nodeOne, Qt::AlignBottom);     //下方：Qt::AlignBottom，左边：Qt::AlignLeft
    chart_nodeOne->addAxis(axisY_nodeOne_temp, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_nodeOne_humi = new QValueAxis();
    axisY_nodeOne_humi->setRange(0, MAX_Y_HUMI); //湿度范围 0-99
    axisY_nodeOne_humi->setTickCount(20);
    axisY_nodeOne_humi->setTitleFont(font);
    axisY_nodeOne_humi->setTitleText("湿度值");
    chart_nodeOne->addAxis(axisY_nodeOne_humi, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop

    // == 坐标轴设置 ==
    axisY_nodeOne_light = new QValueAxis();
    axisY_nodeOne_light->setRange(0, MAX_Y_LIGHT);//光照范围 0-499
    axisY_nodeOne_light->setTickCount(20);
    axisY_nodeOne_light->setTitleFont(font);
    axisY_nodeOne_light->setTitleText("光照值");
    chart_nodeOne->addAxis(axisY_nodeOne_light, Qt::AlignLeft);       //右边：Qt::AlignRight，上方：Qt::AlignTop


    // == 坐标轴设置 ==
    axisY_nodeOne_smog = new QValueAxis();
    axisY_nodeOne_smog->setRange(0, MAX_Y_SMOG);
    axisY_nodeOne_smog->setTickCount(20);
    axisY_nodeOne_smog->setTitleFont(font);
    axisY_nodeOne_smog->setTitleText("烟雾值");
    chart_nodeOne->addAxis(axisY_nodeOne_smog, Qt::AlignLeft);

    Series_nodeOne_One->attachAxis(axisX_nodeOne);       // - 把曲线关联到坐标轴
    Series_nodeOne_One->attachAxis(axisY_nodeOne_temp);
    Series_nodeOne_Two->attachAxis(axisX_nodeOne);
    Series_nodeOne_Two->attachAxis(axisY_nodeOne_humi);
    Series_nodeOne_Thr->attachAxis(axisX_nodeOne);
    Series_nodeOne_Thr->attachAxis(axisY_nodeOne_light);
    Series_nodeOne_Fou->attachAxis(axisX_nodeOne);
    Series_nodeOne_Fou->attachAxis(axisY_nodeOne_smog);


    Series_nodeOne_One->show();
    Series_nodeOne_Two->hide();
    Series_nodeOne_Thr->hide();
    Series_nodeOne_Fou->hide();
    axisY_nodeOne_temp->show();
    axisY_nodeOne_humi->hide();
    axisY_nodeOne_light->hide();
    axisY_nodeOne_smog->hide();
    // == 把chart显示到窗口上 ==
    ui->graphicsView->setChart(chart_nodeOne);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);// 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑
    chart_nodeOne->setTheme(QChart::ChartThemeBlueIcy);
}



void node1::on_back_1Button_clicked()
{
    //this->close();
     emit Signal_Return();
}

void node1::Slot_RefreshTime()
{
    ui->label_time->setText(stringCurrentTime);
}

bool mutex_log = 0;
//unsigned char mutex_power1;
void node1::Slot_Warning(unsigned int error_index)
{
    if(state_nodeOne)//如果节点1上线
    {
        if(mutex_log == 0)//如果之前是没上线的
        {
            mutex_log = 1;//已上线
            ui->textEdit_log->setTextColor(Qt::blue);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点一已上线！");//ui->节点日志显示已上线

            node_number++;
            node_disnumber--;
        }
        if(error_index & 0x01)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 温度异常！");
            ui->textEdit_temp->setTextColor(Qt::red);  //温度字体变红


            if(flag1==0)//flag1 控制只发一次警告信号
            {
                flag1=1;
                emit Node1_Danger();

            }

        }
        else
        {
            ui->textEdit_temp->setTextColor(Qt::black);//字体黑
            error1 = 0;
            if(flag1==1)
            {
                flag1=0;
                emit Node1_Safe();

            }
        }


        if(error_index & 0x02)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 湿度异常！");
            ui->textEdit_humi->setTextColor(Qt::red);


            if(flag1==0)
            {
                flag1=1;
                emit Node1_Danger();

            }
        }
        else
        {
            ui->textEdit_humi->setTextColor(Qt::black);
            error2 = 0;
            if(flag1==1)
            {
                flag1=0;
                emit Node1_Safe();

            }
        }
        if(error_index & 0x04)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 光照异常！");
            ui->textEdit_light->setTextColor(Qt::red);


            if(flag1==0)
            {
                flag1=1;
                emit Node1_Danger();

            }
        }
        else
        {
            ui->textEdit_light->setTextColor(Qt::black);
            error3 = 0;
            if(flag1==1)
            {
                flag1=0;
                emit Node1_Safe();

            }
        }


        if(error_index & 0x1000)
        {
            ui->textEdit_log->setTextColor(Qt::red);
            ui->textEdit_log->append(stringCurrentTimeMessage + ":烟雾颗粒超标！");
            ui->textEdit_smog->setTextColor(Qt::red);


            if(flag1==0)
            {
                flag1=1;
                emit Node1_Danger();

            }
        }
        else
        {
            ui->textEdit_smog->setTextColor(Qt::black);
            error13 = 0;
            if(flag1==1)
            {
                flag1=0;
                emit Node1_Safe();

            }
        }

        if((error_index & 0x1007) == 0)
        {


            if(flag1==0)
            {
                flag1=1;
                emit Node1_Danger();

            }
        }


        ui->textEdit_temp->append(stringCurrentTimeMessage + ": " + stringTemp1 + "℃");
        ui->textEdit_humi->append(stringCurrentTimeMessage + ": " + stringHumi1 + "%RH");
        ui->textEdit_light->append(stringCurrentTimeMessage + ": " + stringLight1 + "lx");
        ui->textEdit_smog->append(stringCurrentTimeMessage + ": " + stringSmog1 + "mg");
    }

}

void node1::downline()
{
    if(!state_nodeOne)
    {
        if(mutex_log)//如果之前是上线的
        {
            mutex_log = 0;
            ui->textEdit_log->setTextColor(Qt::black);
            ui->textEdit_log->append(stringCurrentTimeMessage + ": 节点一已下线！");

            node_number--;
            node_disnumber++;
            error1 = error2 = error3 = error10 =error13 = 0;
        }
    }

}

void node1::on_temperature1Button_clicked()
{
    Series_nodeOne_One->show();
    Series_nodeOne_Two->hide();
    Series_nodeOne_Thr->hide();
    axisY_nodeOne_temp->show();
    axisY_nodeOne_humi->hide();
    axisY_nodeOne_light->hide();
    axisY_nodeOne_smog->hide();
    Series_nodeOne_Fou->hide();
}


void node1::on_humi_1Button_clicked()
{
    Series_nodeOne_One->hide();
    Series_nodeOne_Two->show();
    Series_nodeOne_Thr->hide();
    axisY_nodeOne_temp->hide();
    axisY_nodeOne_humi->show();
    axisY_nodeOne_light->hide();
    axisY_nodeOne_smog->hide();
    Series_nodeOne_Fou->hide();
}


void node1::on_light_1Button_clicked()
{
    Series_nodeOne_One->hide();
    Series_nodeOne_Two->hide();
    Series_nodeOne_Thr->show();
    axisY_nodeOne_temp->hide();
    axisY_nodeOne_humi->hide();
    axisY_nodeOne_light->show();
    axisY_nodeOne_smog->hide();
    Series_nodeOne_Fou->hide();
}


void node1::on_smog_1Button_clicked()
{
    Series_nodeOne_One->hide();
    Series_nodeOne_Two->hide();
    Series_nodeOne_Thr->hide();
    Series_nodeOne_Fou->show();
    axisY_nodeOne_temp->hide();
    axisY_nodeOne_humi->hide();
    axisY_nodeOne_light->hide();
    axisY_nodeOne_smog->show();
}


void node1::on_pushButton_data_clicked()
{
    if (ui->textEdit_temp->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "提示", "数据内容空");
        return;
    }
    QString data;
    data.append("节点一温度值:");
    data.append("\r\n");
    data.append(ui->textEdit_temp->toPlainText());
    data.append("\r\n");
    data.append("节点一湿度值:");
    data.append("\r\n");
    data.append(ui->textEdit_humi->toPlainText());
    data.append("\r\n");
    data.append("节点一光照值:");
    data.append("\r\n");
    data.append(ui->textEdit_light->toPlainText());
    data.append("\r\n");
    data.append("节点一烟雾值:");
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

