#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QString>
#include <QDate>
#include "weatherTool.h"
#include <QPainter>
#include <QPoint>

#define INCREMENT 3//温度每升高/降低1°,y轴坐标的增量
// ,(因为界面的y轴是向下的,原点在界面左上角,向上偏移就是-,向下偏移就是+)
#define POINT_RADIUS 3//曲线描点的大小
#define TEXT_OFFSET_X 12//x,y轴的偏移
#define TEXT_OFFSET_Y 12


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 加上这一句：强制让 QWidget 遵循样式表去绘制背景
    this->setAttribute(Qt::WA_StyledBackground, true);

    //1. 设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setFixedSize(width(),height());         //设置固定窗口大小

    //2. 构建右键菜单,退出程序
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();

    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);

    connect(mExitAct,&QAction::triggered,this,[=](){
        qApp->exit(0);
    });

    //将控件添加到控件数组
    //星期和日期
    mWeekList<<ui->lblWeek0<<ui->lblWeek1<<ui->lblWeek2<<ui->lblWeek3<<ui->lblWeek4<<ui->lblWeek5;
    mDateList<<ui->lblDate0<<ui->lblDate1<<ui->lblDate2<<ui->lblDate3<<ui->lblDate4<<ui->lblDate5;
    //天气和天气图标
    mTypeList<<ui->lblType0<<ui->lblType1<<ui->lblType2<<ui->lblType3<<ui->lblType4<<ui->lblType5;
    mTypeIconList<<ui->lblTypeIcon0<<ui->lblTypeIcon1<<ui->lblTypeIcon2<<ui->lblTypeIcon3<<ui->lblTypeIcon4<<ui->lblTypeIcon5;
    //天气指数
    mAqiList<<ui->lblquality0<<ui->lblquality1<<ui->lblquality2<<ui->lblquality3<<ui->lblquality4<<ui->lblquality5;
    //风力和风向
    mFxList<<ui->lblFX0<<ui->lblFX1<<ui->lblFX2<<ui->lblFX3<<ui->lblFX4<<ui->lblFX5;
    mFlList<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<<ui->lblFl4<<ui->lblFl5;

    //图标
    mTypeMap.insert("暴雪",":/res/type/baoxue.png");
    mTypeMap.insert("暴雨",":/res/type/baoyu.png");
    mTypeMap.insert("多云",":/res/type/duoyun.png");
    mTypeMap.insert("冻雨",":/res/type/dongyu.png");
    mTypeMap.insert("大雨",":/res/type/dayu.png");
    mTypeMap.insert("大雪",":/res/type/daxue.png");
    mTypeMap.insert("大风",":/res/type/dafeng.png");
    mTypeMap.insert("风暴",":/res/type/fengbao.png");
    mTypeMap.insert("浮尘",":/res/type/fuchen.png");
    mTypeMap.insert("和风",":/res/type/hefeng.png");
    mTypeMap.insert("疾风",":/res/type/jifeng.png");
    mTypeMap.insert("飓风",":/res/type/jufeng.png");
    mTypeMap.insert("狂风",":/res/type/kuangbaofeng.png");
    mTypeMap.insert("雷阵雨",":/res/type/leizhenyu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/leizhenyubanyoubingbao.png");
    mTypeMap.insert("冷",":/res/type/leng.png");
    mTypeMap.insert("烈风",":/res/type/liefeng.png");
    mTypeMap.insert("龙卷风",":/res/type/longjuanfeng.png");
    mTypeMap.insert("霾",":/res/type/mai.png");
    mTypeMap.insert("浓雾",":/res/type/nongwu.png");
    mTypeMap.insert("平静",":/res/type/pingjing.png");
    mTypeMap.insert("强风/劲风",":/res/type/qiangfengjinfeng.png");
    mTypeMap.insert("强浓雾",":/res/type/qiangnongwu.png");
    mTypeMap.insert("强沙尘暴",":/res/type/qiangshachenbao.png");
    mTypeMap.insert("轻风",":/res/type/qingfeng.png");
    mTypeMap.insert("轻雾",":/res/type/qingwu.png");
    mTypeMap.insert("热带风暴",":/res/type/redaifengbao.png");
    mTypeMap.insert("热",":/res/type/re.png");
    mTypeMap.insert("沙尘暴",":/res/type/shachenbao.png");
    mTypeMap.insert("特大暴雨",":/res/type/tedabaoyubaoyu.png");
    mTypeMap.insert("晴",":/res/type/tq-1.png");
    mTypeMap.insert("微风",":/res/type/weifeng.png");
    mTypeMap.insert("未知",":/res/type/weizhi.png");
    mTypeMap.insert("雾",":/res/type/wu.png");
    mTypeMap.insert("小雪",":/res/type/xiaoxue.png");
    mTypeMap.insert("小雨",":/res/type/xiaoyu.png");
    mTypeMap.insert("扬沙",":/res/type/yangsha.png");
    mTypeMap.insert("严重霾",":/res/type/yanzhongmai.png");
    mTypeMap.insert("夜晚",":/res/type/yewan.png");
    mTypeMap.insert("夜间多云",":/res/type/yejianduoyun.png");
    mTypeMap.insert("阴",":/res/type/yin.png");
    mTypeMap.insert("有风",":/res/type/youfeng.png");
    mTypeMap.insert("雨夹雪",":/res/type/yujiaxue.png");
    mTypeMap.insert("阵雪",":/res/type/zhenxue.png");
    mTypeMap.insert("阵雨",":/res/type/zhenyu.png");
    mTypeMap.insert("中度霾",":/res/type/zhongdumai.png");
    mTypeMap.insert("中雪",":/res/type/zhongxue.png");
    mTypeMap.insert("中雨",":/res/type/zhongyu.png");

    //3. 网络请求
    //关联信号和槽,当getWeatherInfo发送http请求完毕后,服务器返回数据,此时:
    //mNetAccessManger就会发送一个finished信号,并将QNetworkReply的指针携带服务器的响应,作为参数传递出去,
    //进而调用onReplied槽函数作出 处理响应动作
    mNetAccessManger = new QNetworkAccessManager(this);
    connect(mNetAccessManger,&QNetworkAccessManager::finished,this,&Widget::onReplied);

    // 发送http请求: 直接在构造中请求天气数据
    //城市编码https://www.weather.com.cn/中国天气网,选择某个城市的40天预报后上方显示的URL的数字即为城市编码
    // getWeatherInfo("101010100");// 北京的城市编码
    // getWeatherInfo("101280101");// 广州的城市编码
    // getWeatherInfo("101091011");//鸡泽的城市编码
    // getWeatherInfo("101090101");//石家庄的城市编码
    // getWeatherInfo("101020100");//上海的城市编码
    // getWeatherInfo("合肥");
    getLocationByIP();           // ← 改为自动定位

    //(1)安装事件过滤器 给标签添加事件过滤器
    // 参数指定为this,也就是当前窗口对象mainwindow
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);


}

Widget::~Widget()
{
    delete ui;
}

//重写父类的虚函数,父类中默认的实现 是忽略右键菜单事件,重写之后就可以处理右键菜单
void Widget::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());

    //调用accpet表示:这个事件我已经处理了,不需要向上传递了
    event->accept();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    //event->globalPos() 是鼠标在屏幕上的全局坐标。
    //this->pos() 是当前窗口左上角在屏幕上的全局坐标
    //两者相减，得到鼠标点击位置到窗口左上角的向量，保存在成员变量 mOffset 中。
    //鼠标到窗口左上角偏移量 mOffset
    // Qt6 兼容写法
    mOffset = event->globalPosition().toPoint() - this->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //event->globalPos() 得到鼠标移动后的新坐标
    //此时窗口应该在:该鼠标新位置-鼠标到窗口左上角那个固定的偏移量
    this->move(event->globalPosition().toPoint() - mOffset);
}

//发送http请求
// void Widget::getWeatherInfo(QString cityCode)
// {
//     QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
//     mNetAccessManger->get(QNetworkRequest(url));//发送一个 HTTP GET 请求。
//     //QNetworkAccessManager 会根据你的请求，在内部创建一个 QNetworkReply 对象来处理这个具体的网络事务。

// }
void Widget::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);

    if(cityCode.isEmpty()){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("天气");
        msgBox.setText("请检查输入的城市是否正确!(省级以下的城市)");
        msgBox.setStyleSheet("color: black;");
        msgBox.exec();
        // QMessageBox::warning(this,"天气","请检查输入的城市是否正确!",QMessageBox::Ok);
        return;
    }

    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
    mNetAccessManger->get(QNetworkRequest(url));//发送一个 HTTP GET 请求。
    //QNetworkAccessManager 会根据你的请求，在内部创建一个 QNetworkReply 对象来处理这个具体的网络事务。

}

//解析JSON并调用标签的update方法
void Widget::parseJson(QByteArray &byteArray)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&error);
    if(error.error !=QJsonParseError::NoError){
        return;
    }
    //解析成JSON对象
    QJsonObject rootObj = doc.object();
    qDebug()<<rootObj.value("message").toString();

    if(rootObj.value("status").toInt() != 200){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("天气");
        msgBox.setText("请检查输入的城市是否正确!(省级以下的城市)");
        msgBox.setStyleSheet("color: black;");
        msgBox.exec();
        return;
    }

    //1. 解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();
    //2. 解析 yesterday
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonObject ojbYesterday = objData.value("yesterday").toObject();
    mDay[0].week = ojbYesterday.value("week").toString();
    mDay[0].date = ojbYesterday.value("ymd").toString();
    mDay[0].type = ojbYesterday.value("type").toString();

    QString s;
    {
        QStringList parts = ojbYesterday.value("high").toString().split(" ");
        s = parts.size() > 1 ? parts.at(1) : "";
    }
    s = s.left(s.length()-1);
    mDay[0].high = s.toInt();
    {
        QStringList parts = ojbYesterday.value("low").toString().split(" ");
        s = parts.size() > 1 ? parts.at(1) : "";
    }
    s = s.left(s.length()-1);
    mDay[0].low = s.toInt();

    //风向和风力
    mDay[0].fl = ojbYesterday.value("fl").toString();
    mDay[0].fx = ojbYesterday.value("fx").toString();
    //aqi天气指数/污染指数
    mDay[0].aqi = ojbYesterday.value("aqi").toDouble();

    //3. 解析 forecast 中5天的数据
    QJsonArray forecastArray = objData.value("forecast").toArray();
    for(int i = 0;i<5;i++)
    {
        QJsonObject objForecast = forecastArray[i].toObject();
        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();

        {
            QStringList parts = objForecast.value("high").toString().split(" ");
            s = parts.size() > 1 ? parts.at(1) : "";
        }
        s = s.left(s.length()-1);
        mDay[i+1].high = s.toInt();

        {
            QStringList parts = objForecast.value("low").toString().split(" ");
            s = parts.size() > 1 ? parts.at(1) : "";
        }
        s = s.left(s.length()-1);
        mDay[i+1].low = s.toInt();

        //风向和风力
        mDay[i+1].fl = objForecast.value("fl").toString();
        mDay[i+1].fx = objForecast.value("fx").toString();
        //aqi天气指数/污染指数
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();
    }
    //4. 解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();

    QJsonValue wenduVal = objData.value("wendu");
    mToday.wendu = wenduVal.toString().toDouble();

    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();

    //5. forecast中第一个数组元素也是今天的数据
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //6. 更新UI
    //6.1 显示文本和图标
    updataUI();
    //6.2 绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
    //调用标签的update方法,框架发送QEvent::Paint事件 给标签,事件被MainWindow拦截,进而调用eventfilter方法
    //在eventfilter中,调用paintHighCurve和paintLowCurve来进行真正的绘制曲线
}

void Widget::updataUI()
{
    //1. 更新日期和城市
    QDate date = QDate::fromString(mToday.date, "yyyyMMdd");//20221022
    ui->lblDate->setText(
        QString("<p><span style='font-size:16pt; font-weight:700;'>%1 %2</span></p>")
            .arg(date.toString("yyyy/MM/dd"))
            .arg(mDay[1].week)
        );
    ui->lblCity->setText(
        QString("<p align='center'><span style='font-size:16pt;'>%1</span></p>")
            .arg(mToday.city)
        );

    //2. 更新今天
    {
        QString iconPath = mTypeMap.value(mToday.type);
        if (iconPath.isEmpty()) {
            QStringList parts = mToday.type.split("转");
            iconPath = mTypeMap.value(parts[0], ":/res/type/tq-1.png");
        }
        ui->lblTypeIcon->setStyleSheet("");
        ui->lblTypeIcon->setPixmap(QPixmap(iconPath));
    }
    ui->lblTemp->setText(
        QString("<html><head/><body><p align='center'>"
                "<span style='font-size:35pt; font-weight:700;'>%1℃</span>"
                "</p></body></html>")
            .arg(mToday.wendu,0, 'f', 1)
        );
    ui->lblType->setText(
        QString("<p align='center'><span style='font-size:14pt;'>%1</span></p>")
            .arg(mToday.type)
        );
    // 温度范围（原字号 14pt，居中）
    ui->lblLowHigh->setText(
        QString("<p align='center'><span style='font-size:14pt;'>%1℃~%2℃</span></p>")
            .arg(mToday.low)
            .arg(mToday.high)
        );

    ui->lblGanmao->setText(QString("<p align='left'><span style='font-size:20pt;'>感冒指数:%1</span></p>").arg(mToday.ganmao));
    // ui->lblGanmao->setText(""+mToday.ganmao);
    ui->lblWindFx->setText(QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>").arg(mToday.fx));
    ui->lblWindFl->setText(QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>").arg(mToday.fl));
    ui->lblPM25->setText(QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>").arg(QString::number(mToday.pm25)));
    ui->lblShidu->setText(QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>").arg(mToday.shidu));
    double todayAqi = mDay[1].aqi;
    QString quality;
    if (todayAqi <= 50) { quality = "优"; ui->lblQuality->setStyleSheet("background-color: rgb(0, 228, 0);color: rgb(255, 255, 255);"); }
    else if (todayAqi <= 100) { quality = "良"; ui->lblQuality->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);"); }
    else if (todayAqi <= 150) { quality = "轻度"; ui->lblQuality->setStyleSheet("background-color: rgb(255, 126, 0);color: rgb(255, 255, 255);"); }
    else if (todayAqi <= 200) { quality = "中度"; ui->lblQuality->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);"); }
    else if (todayAqi <= 300) { quality = "重度"; ui->lblQuality->setStyleSheet("background-color: rgb(153, 0, 76);color: rgb(255, 255, 255);"); }
    else { quality = "严重"; ui->lblQuality->setStyleSheet("background-color: rgb(126, 0, 35);color: rgb(255, 255, 255);"); }
    ui->lblQuality->setText(QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>").arg(quality));

    //3. 更新六天
    for(int i = 0;i<6;i++){
        //3.1 更新日期和时间(周几)
        mWeekList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg("周" + mDay[i].week.right(1))
            );

        ui->lblWeek0->setText("<p align='center'><span style='font-size:12pt;'>昨天</span></p>");
        ui->lblWeek1->setText("<p align='center'><span style='font-size:12pt;'>今天</span></p>");
        ui->lblWeek2->setText("<p align='center'><span style='font-size:12pt;'>明天</span></p>");

        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg(ymdList.size() > 2 ? ymdList[1] + "/" + ymdList[2] : "")
            );

        //3.2 更新天气类型
        {
            QString iconPath = mTypeMap.value(mDay[i].type);
            if (iconPath.isEmpty()) {
                QStringList parts = mDay[i].type.split("转");
                iconPath = mTypeMap.value(parts[0], ":/res/type/tq-1.png");
            }
            mTypeIconList[i]->setStyleSheet("border-image: none;");
            mTypeIconList[i]->setPixmap(QPixmap(iconPath));
        }
        mTypeList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg(mDay[i].type)
            );
        //图标

        //3.3 更新天气质量
        QString quality;
        if (mDay[i].aqi <= 50) {
            quality = "优";
            mAqiList[i]->setStyleSheet("background-color: rgb(0, 228, 0);color: rgb(255, 255, 255);");
        } else if (mDay[i].aqi <= 100) {
            quality = "良";
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);");
        } else if (mDay[i].aqi <= 150) {
            quality = "轻度";
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 126, 0);color: rgb(255, 255, 255);");
        } else if (mDay[i].aqi <= 200) {
            quality = "中度";
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
        } else if (mDay[i].aqi <= 300) {
            quality = "重度";
            mAqiList[i]->setStyleSheet("background-color: rgb(153, 0, 76);color: rgb(255, 255, 255);");
        } else {
            quality = "严重";
            mAqiList[i]->setStyleSheet("background-color: rgb(126, 0, 35);color: rgb(255, 255, 255);");
        }
        mAqiList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg(quality)
            );

        //3.4更新风力和风向
        mFxList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg(mDay[i].fx)
            );
        mFlList[i]->setText(
            QString("<p align='center'><span style='font-size:12pt;'>%1</span></p>")
                .arg(mDay[i].fl)
            );
    }
}

//实现eventsfilter方法重写
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->lblHighCurve && event->type()==QEvent::Paint){
        paintHighCurve();   // 保安拦下“重绘事件”，亲自画了温度曲线
    }
    if(watched==ui->lblLowCurve && event->type()==QEvent::Paint){
        paintLowCurve();    // 保安拦下“重绘事件”，亲自画了温度曲线
    }
    // return QWidget::eventFilter(watched,event);//事件继续往后传
    return true;//这样事件就被“保安”吃掉了，员工再也收不到快递。
}

//最高温曲线绘图
void Widget::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1. 获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i<6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;

    }
    //2. 获取y坐标
    int tempSum = 0;
    int tempAvearge = 0;
    for(int i = 0;i<6;i++){
        tempSum += mDay[i].high;
    }
    tempAvearge = tempSum/6;//最高温的平均值//平均值在label中间

    //计算y坐标
    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i = 0;i<6;i++){
        pointY[i] = yCenter - ((mDay[i].high-tempAvearge)*INCREMENT);
    }

    //3. 开始绘制
    //3.1 初始化画笔相关
    painter.save();                     // ① 保存画家默认状态(此时画笔还是默认黑色，因为还没setPen

    QPen pen = painter.pen();           // ② 获取当前画笔的副本（默认黑色、1px等）
    pen.setWidth(1);                    // ③ 修改画笔副本（宽度设为1）//设置画笔的宽度
    pen.setColor(QColor(255,170,0));    // ④ 修改画笔副本（颜色设为橙色）//设置画笔的颜色

    painter.setPen(pen);                // ⑤ 设置画家的画笔
    painter.setBrush(QColor(255,170,0));// ⑥ 设置画家画刷的颜色-内部填充的颜色


    //3.2 画点、写文字
    for(int i = 0;i<6;i++){
        //显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,
                         QString::number(mDay[i].high) + "°");
    }

    //3.3 绘制曲线
    for(int i = 0;i<5;i++){
        //昨天到今天是虚线
        if(i == 0){
            pen.setStyle(Qt::DotLine);//虚线
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);//实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }
    painter.restore();       // ⑦ 恢复save()原始状态
}
//最低温曲线绘图
void Widget::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1. 获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i<6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;

    }
    //2. 获取y坐标
    int tempSum = 0;
    int tempAvearge = 0;
    for(int i = 0;i<6;i++){
        tempSum += mDay[i].low;
    }
    tempAvearge = tempSum/6;//最高温的平均值//平均值在label中间

    //计算y坐标
    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i = 0;i<6;i++){
        pointY[i] = yCenter - ((mDay[i].low-tempAvearge)*INCREMENT);
    }

    //3. 开始绘制
    //3.1 初始化画笔相关
    painter.save();                     // ① 保存画家默认状态(此时画笔还是默认黑色，因为还没setPen

    QPen pen = painter.pen();           // ② 获取当前画笔的副本（默认黑色、1px等）
    pen.setWidth(1);                    // ③ 修改画笔副本（宽度设为1）//设置画笔的宽度
    pen.setColor(QColor(0,255,255));    // ④ 修改画笔副本（颜色设为蓝色）//设置画笔的颜色

    painter.setPen(pen);                // ⑤ 设置画家的画笔
    painter.setBrush(QColor(0,255,255));// ⑥ 设置画家画刷的颜色-内部填充的颜色

    //3.2 画点、写文字
    for(int i = 0;i<6;i++){
        //显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,
                         QString::number(mDay[i].high) + "°");
    }

    //3.3 绘制曲线
    for(int i = 0;i<5;i++){
        //昨天到今天是虚线
        if(i == 0){
            pen.setStyle(Qt::DotLine);//虚线
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);//实线
            painter.setPen(pen);
        }

        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }
    painter.restore();        // ⑦ 恢复save()原始状态
}

//槽函数用于处理服务器返回的数据
void Widget::onReplied(QNetworkReply *reply)
{
    qDebug()<<"onReplied success";

    //当响应的状态码为200时,表示请求成功
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"operation:"<<reply->operation();     //请求方式
    qDebug()<<"status_code:"<<status_code;          //状态码
    qDebug()<<"url:"<<reply->url();                 //url
    qDebug()<<"raw header:"<<reply->rawHeaderList();//header

    //判断请求是否出错（如网络不通、DNS 解析失败）或 HTTP 状态码不是 200 OK。
    if(reply->error() != QNetworkReply::NoError || status_code != 200)
    {
        qDebug()<<reply->errorString().toLatin1().data();
        //如果是失败状态，则弹出警告框提示“请求数据失败”。
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("天气");
        msgBox.setText("请求数据失败，请检查城市编码~~！");
        msgBox.setStyleSheet("color: black;");
        msgBox.exec();
    }
    else
    {
        //获取响应信息
        //用 reply->readAll() 读取服务器返回的完整数据（这里就是天气的 JSON 字符串），并打印出来。
        QByteArray byteArray = reply->readAll();
        qDebug()<<"read all:"<<byteArray.data();
        parseJson(byteArray);
    }
    //最后必须调用 reply->deleteLater() 释放网络回复对象的内存，避免内存泄漏。
    reply->deleteLater();
}

void Widget::on_pushButton_2_clicked()
{
    QString cityName = ui->lineEdit_2->text();
    getWeatherInfo(cityName);
}

void Widget::getLocationByIP()
{
    auto *ipManager = new QNetworkAccessManager(this);
    connect(ipManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        reply->deleteLater();
        ipManager->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            getWeatherInfo("北京");        // 失败时默认
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QString city = doc.object().value("city").toString();
        qDebug() << "自动定位到:" << city;
        getWeatherInfo(city.isEmpty() ? "北京" : city);
    });
    ipManager->get(QNetworkRequest(QUrl("http://ip-api.com/json/?lang=zh-CN")));
}

