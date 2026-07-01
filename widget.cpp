#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QString>
#include <QDate>
#include <QPoint>
#include <QSettings>
#include <QTimer>
#include <QApplication>
#include "weatherTool.h"

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
    setWindowIcon(QIcon("://res/WeatherForecast.png"));

    //2. 构建右键菜单,退出程序
    mExitMenu = new QMenu(this);
    mExitMenu->setStyleSheet("QMenu { color: black; } QMenu::item { color: black; }");

    m_minimizeAct = new QAction(QIcon("://res/MiniWindow.png"), "最小化到托盘");
    mExitMenu->addAction(m_minimizeAct);

    mExitMenu->addSeparator();

    mExitAct = new QAction();
    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);

    connect(m_minimizeAct, &QAction::triggered, this, [this]() {
        hide();
        m_trayIcon->show();
    });
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

    m_cityDropdown = new QListWidget;
    m_cityDropdown->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    m_cityDropdown->setFocusPolicy(Qt::NoFocus);
    m_cityDropdown->setFrameShape(QFrame::NoFrame);
    m_cityDropdown->setStyleSheet(
        "QListWidget { background: white; border: 1px solid #ccc; color: black; font-size: 12pt; }"
        "QListWidget::item { padding: 6px 12px; }"
        "QListWidget::item:hover { background: #e0e0e0; }"
    );
    m_cityDropdown->hide();
    connect(m_cityDropdown, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item->data(Qt::UserRole).toString() == "__clear__") {
            m_cache->clearAll();
            QSettings().remove("lastCityCode");
            m_cityDropdown->hide();
            return;
        }
        m_cityDropdown->hide();
        ui->lineEdit_2->setText(item->text());
        on_pushButton_2_clicked();
    });
    qApp->installEventFilter(this);

    ui->lineEdit_2->setContextMenuPolicy(Qt::NoContextMenu);
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, this, &Widget::on_pushButton_2_clicked);

    //3. 网络请求
    //关联信号和槽,当getWeatherInfo发送http请求完毕后,服务器返回数据,此时:
    //mNetAccessManger就会发送一个finished信号,并将QNetworkReply的指针携带服务器的响应,作为参数传递出去,
    //进而调用onReplied槽函数作出 处理响应动作
    m_apiClient = new ApiClient(this);
    connect(m_apiClient, &ApiClient::weatherDataReady,
            this, &Widget::onWeatherDataReady);

    connect(m_apiClient, &ApiClient::errorOccurred,this, [](const QString &msg) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("天气");
        msgBox.setText(msg);
        msgBox.setStyleSheet("color: black;");
        msgBox.exec();
     });

    m_cache = new DataCache(this);

    QSettings settings;
    QString lastCityCode = settings.value("lastCityCode").toString();
    if (!lastCityCode.isEmpty() && m_cache->load(lastCityCode, mToday, mDay))
        updataUI();
    else
        showDefaultUI();

    QTimer::singleShot(0, this, [this, lastCityCode]() {
        if (!lastCityCode.isEmpty())
            m_apiClient->getWeatherInfo(lastCityCode);
        else
            m_apiClient->getLocationByIP();
    });

    ui->lblGanmao->setWordWrap(true);

    m_highChart = new ChartWidget(QColor(255, 170, 0), ui->widget_5);
    m_highChart->setGeometry(0, 0, 351, 81);
    m_lowChart = new ChartWidget(QColor(0, 255, 255), ui->widget_5);
    m_lowChart->setGeometry(0, 70, 361, 81);

    QSettings s;
    QPoint pos = s.value("windowPos").toPoint();
    if (!pos.isNull()) {
        QTimer::singleShot(0, this, [this, pos]() {
            QScreen *screen = QGuiApplication::primaryScreen();
            if (screen && screen->availableGeometry().contains(pos))
                move(pos);
        });
    }

    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        QSettings s;
        s.setValue("windowPos", this->pos());
    });

    m_trayIcon = new QSystemTrayIcon(QIcon("://res/WeatherForecast.png"), this);
    m_trayIcon->setToolTip("WeatherForecast");

    QMenu *trayMenu = new QMenu(this);
    trayMenu->setStyleSheet("QMenu { color: black; } QMenu::item { color: black; }");
    QAction *showAct = trayMenu->addAction("显示");
    QAction *trayExitAct = trayMenu->addAction("退出");
    m_trayIcon->setContextMenu(trayMenu);

    connect(showAct, &QAction::triggered, this, [this]() {
        showNormal();
        raise();
        activateWindow();
    });

    connect(trayExitAct, &QAction::triggered, this, [this]() {
        m_trayIcon->hide();
        qApp->exit(0);
    });

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            showNormal();
            raise();
            activateWindow();
        }
    });
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

void Widget::updataUI()
{
    //1. 更新日期和城市
    QDate date = QDate::fromString(mToday.date, "yyyyMMdd");//20221022
    ui->lblDate->setText(
        QString("<p><span style='font-size:16pt; font-weight:700;'>%1 %2</span></p>")
            .arg(date.toString("yyyy/MM/dd"),mDay[1].week)
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
        auto it = m_iconCache.constFind(iconPath);
        if (it == m_iconCache.constEnd())
            it = m_iconCache.insert(iconPath, QPixmap(iconPath));
        ui->lblTypeIcon->setPixmap(it.value());
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
            .arg(mToday.low,mToday.high)
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
            auto it = m_iconCache.constFind(iconPath);
            if (it == m_iconCache.constEnd())
                it = m_iconCache.insert(iconPath, QPixmap(iconPath));
            mTypeIconList[i]->setPixmap(it.value());
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

void Widget::showDefaultUI()
{
    ui->lblDate->setText(
        "<p><span style='font-size:16pt; font-weight:700;'>----/--/-- --</span></p>");
    ui->lblCity->setText(
        "<p align='center'><span style='font-size:16pt;'>加载中...</span></p>");

    ui->lblTypeIcon->setStyleSheet("");
    ui->lblTypeIcon->setPixmap(QPixmap(":/res/type/tq-1.png"));
    ui->lblTemp->setText(
        "<html><head/><body><p align='center'>"
        "<span style='font-size:35pt; font-weight:700;'>--℃</span>"
        "</p></body></html>");
    ui->lblType->setText(
        "<p align='center'><span style='font-size:14pt;'>--</span></p>");
    ui->lblLowHigh->setText(
        "<p align='center'><span style='font-size:14pt;'>--℃~--℃</span></p>");

    ui->lblGanmao->setText(
        "<p align='left'><span style='font-size:20pt;'>感冒指数:--</span></p>");
    ui->lblWindFx->setText(
        "<p align='center'><span style='font-size:12pt;'>--</span></p>");
    ui->lblWindFl->setText(
        "<p align='center'><span style='font-size:12pt;'>--</span></p>");
    ui->lblPM25->setText(
        "<p align='center'><span style='font-size:12pt;'>--</span></p>");
    ui->lblShidu->setText(
        "<p align='center'><span style='font-size:12pt;'>--</span></p>");
    ui->lblQuality->setStyleSheet("background-color: rgb(180, 180, 180);color: rgb(255, 255, 255);");
    ui->lblQuality->setText(
        "<p align='center'><span style='font-size:12pt;'>--</span></p>");

    ui->lblWeek0->setText("<p align='center'><span style='font-size:12pt;'>昨天</span></p>");
    ui->lblWeek1->setText("<p align='center'><span style='font-size:12pt;'>今天</span></p>");
    ui->lblWeek2->setText("<p align='center'><span style='font-size:12pt;'>明天</span></p>");

    for (int i = 0; i < 6; i++) {
        mWeekList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--</span></p>");
        mDateList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--/--</span></p>");
        mTypeIconList[i]->setStyleSheet("border-image: none;");
        mTypeIconList[i]->setPixmap(QPixmap(":/res/type/tq-1.png"));
        mTypeList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--</span></p>");
        mAqiList[i]->setStyleSheet("background-color: rgb(180, 180, 180);color: rgb(255, 255, 255);");
        mAqiList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--</span></p>");
        mFxList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--</span></p>");
        mFlList[i]->setText(
            "<p align='center'><span style='font-size:12pt;'>--</span></p>");
    }
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && m_cityDropdown->isVisible()) {
        QWidget *src = qobject_cast<QWidget*>(obj);
        if (src && src != m_cityDropdown && !m_cityDropdown->isAncestorOf(src) && src != ui->lineEdit_2) {
            m_cityDropdown->hide();
        }
    }
    if (obj == ui->lineEdit_2 && event->type() == QEvent::MouseButtonPress) {
        if (m_cityDropdown->isVisible())
            m_cityDropdown->hide();
        else
            showCityDropdown();
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

void Widget::showCityDropdown()
{
    m_cityDropdown->clear();
    auto cities = m_cache->getAllCachedCities();

    if (cities.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无缓存记录");
        emptyItem->setFlags(Qt::ItemIsEnabled);
        emptyItem->setForeground(QColor(160, 160, 160));
        m_cityDropdown->addItem(emptyItem);
    } else {
        for (const auto &pair : qAsConst(cities))
            m_cityDropdown->addItem(pair.second);
        QListWidgetItem *clearItem = new QListWidgetItem("清除缓存记录");
        clearItem->setData(Qt::UserRole, "__clear__");
        clearItem->setForeground(QColor(160, 160, 160));
        m_cityDropdown->addItem(clearItem);
    }

    QPoint pos = ui->lineEdit_2->mapToGlobal(QPoint(0, ui->lineEdit_2->height()));
    m_cityDropdown->setFixedWidth(ui->lineEdit_2->width());
    m_cityDropdown->move(pos);
    m_cityDropdown->show();
}

void Widget::on_pushButton_2_clicked()
{
    QString cityName = ui->lineEdit_2->text();
    QString cityCode = WeatherTool::getCityCode(cityName);
    if (!cityCode.isEmpty()) {
        QSettings settings;
        settings.setValue("lastCityCode", cityCode);
        if (m_cache->load(cityCode, mToday, mDay)) {
            onWeatherDataReady(mToday, mDay);
            return;
        }
    }
    m_apiClient->getWeatherInfo(cityName);
}

void Widget::onWeatherDataReady(const Today &today, const Day day[6])
{
    mToday = today;
    for (int i = 0; i < 6; ++i) mDay[i] = day[i];
    updataUI();

    QSettings settings;
    QString cityCode = WeatherTool::getCityCode(mToday.city);
    if (!cityCode.isEmpty())
        settings.setValue("lastCityCode", cityCode);

    int high[6], low[6];
    for (int i = 0; i < 6; ++i) {
        high[i] = mDay[i].high;
        low[i] = mDay[i].low;
    }
    m_highChart->setData(high);
    m_lowChart->setData(low);

    m_cache->save(cityCode, mToday, mDay);
}


