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
#include <QFontDatabase>
#include <QFile>
#include <QPainter>
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

    loadIconFont();

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
        if (!(item->flags() & Qt::ItemIsSelectable))
            return;
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

    connect(m_apiClient, &ApiClient::errorOccurred, this, [this](const QString &msg) {
        if (m_startupCacheLoaded) {
            m_startupCacheLoaded = false;
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("天气");
            msgBox.setText("网络连接失败，当前显示缓存数据，正在后台重连...");
            msgBox.setStyleSheet("color: black;");
            msgBox.exec();
        } else {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("天气");
            msgBox.setText(msg);
            msgBox.setStyleSheet("color: black;");
            msgBox.exec();
        }
    });

    m_cache = new DataCache(this);

    QSettings settings;
    QString lastCityCode = settings.value("lastCityCode").toString();

    if ((!lastCityCode.isEmpty() && m_cache->load(lastCityCode, mToday, mDay)) ||
        loadRecentCache(lastCityCode)) {
        updataUI();
        m_currentCityCode = lastCityCode;
        m_startupCacheLoaded = true;
    } else
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
        QTimer::singleShot(50, this, [this, pos]() {
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

void Widget::loadIconFont()
{
    int fontId = QFontDatabase::addApplicationFont(":/res/weatherforecast_icon/iconfont.ttf");
    if (fontId >= 0) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty())
            m_iconFont = QFont(families.first(), 40);
    }

    QFile f(":/res/weatherforecast_icon/iconfont.json");
    if (!f.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject())
        return;
    QJsonArray glyphs = doc.object().value("glyphs").toArray();
    for (const QJsonValue &v : glyphs) {
        QJsonObject g = v.toObject();
        QString name = g.value("name").toString();
        QString unicodeHex = g.value("unicode").toString();
        bool ok;
        uint code = unicodeHex.toUInt(&ok, 16);
        if (ok && !name.isEmpty())
            mIconCodeMap.insert(name, QChar(code));
    }

    // API 返回别名
    struct { const char *api; const char *iconfont; } aliases[] = {
        {"狂风", "狂爆风"},
        {"强风/劲风", "强风劲风"},
        {"轻风", "清风"},
        {"雨夹雪", "雨加雪"},
    };
    for (auto &a : aliases) {
        auto it = mIconCodeMap.find(QLatin1String(a.iconfont));
        if (it != mIconCodeMap.end())
            mIconCodeMap.insert(QLatin1String(a.api), it.value());
    }
}

QPixmap Widget::renderWeatherIcon(const QString &weather)
{
    auto it = m_iconCache.constFind(weather);
    if (it != m_iconCache.constEnd())
        return it.value();

    QChar code(0xe695); // 默认"晴"
    auto codeIt = mIconCodeMap.constFind(weather);
    if (codeIt != mIconCodeMap.constEnd())
        code = codeIt.value();

    int size = 64;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    int pointSize = m_iconFont.pointSize();
    if (pointSize <= 0)
        pointSize = 40;
    QFont f(m_iconFont);
    f.setPixelSize(size * 0.75);
    painter.setFont(f);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(QRect(0, 0, size, size), Qt::AlignCenter, QString(code));
    painter.end();

    m_iconCache.insert(weather, pixmap);
    return pixmap;
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
    QDate today = QDate::currentDate();
    QLocale locale("zh_CN");
    ui->lblDate->setText(
        QString("<p><span style='font-size:16pt; font-weight:700;'>%1 %2</span></p>")
            .arg(today.toString("yyyy/MM/dd"), locale.dayName(today.dayOfWeek()))
        );
    ui->lblCity->setText(
        QString("<p align='center'><span style='font-size:16pt;'>%1</span></p>")
            .arg(mToday.city)
        );

    //2. 更新今天
    {
        ui->lblTypeIcon->setStyleSheet("");
        ui->lblTypeIcon->setPixmap(renderWeatherIcon(mToday.type));
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
            .arg(mToday.low).arg(mToday.high)
        );

    ui->lblGanmao->setText(QString("感冒指数: %1").arg(mToday.ganmao));
    ui->lblNotice->setText(QString("温馨提示: %1").arg(
        mDay[1].notice.isEmpty() ? "--" : mDay[1].notice));
    ui->lblSunrise->setText(QString("日出 %1").arg(
        mDay[1].sunrise.isEmpty() ? "--:--" : mDay[1].sunrise));
    ui->lblSunset->setText(QString("日落 %1").arg(
        mDay[1].sunset.isEmpty() ? "--:--" : mDay[1].sunset));
    ui->lblUpdateTime->setText(QString("数据更新: %1").arg(
        mToday.updateTime.isEmpty() ? "--:--" : mToday.updateTime));
    ui->lblPM10->setText(QString("PM10: %1").arg(mToday.pm10));
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
            mTypeIconList[i]->setStyleSheet("border-image: none;");
            mTypeIconList[i]->setPixmap(renderWeatherIcon(mDay[i].type));
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
    ui->lblTypeIcon->setPixmap(renderWeatherIcon("晴"));
    ui->lblTemp->setText(
        "<html><head/><body><p align='center'>"
        "<span style='font-size:35pt; font-weight:700;'>--℃</span>"
        "</p></body></html>");
    ui->lblType->setText(
        "<p align='center'><span style='font-size:14pt;'>--</span></p>");
    ui->lblLowHigh->setText(
        "<p align='center'><span style='font-size:14pt;'>--℃~--℃</span></p>");

    ui->lblGanmao->setText("感冒指数: --");
    ui->lblNotice->setText("温馨提示: --");
    ui->lblSunrise->setText("日出 --:--");
    ui->lblSunset->setText("日落 --:--");
    ui->lblUpdateTime->setText("数据更新: --:--");
    ui->lblPM10->setText("PM10: --");
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
        mTypeIconList[i]->setPixmap(renderWeatherIcon("晴"));
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

bool Widget::loadRecentCache(QString &outCityCode)
{
    auto cities = m_cache->getAllCachedCities();
    if (cities.isEmpty())
        return false;

    outCityCode = cities.first().first;
    return m_cache->load(outCityCode, mToday, mDay);
}

void Widget::on_pushButton_2_clicked()
{
    QString cityName = ui->lineEdit_2->text();
    QString cityCode = WeatherTool::getCityCode(cityName);

    if (!cityCode.isEmpty() && cityCode == m_currentCityCode)
        return;

    m_startupCacheLoaded = false;
    m_apiClient->cancelRetry();

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
    if (!cityCode.isEmpty()) {
        settings.setValue("lastCityCode", cityCode);
        m_currentCityCode = cityCode;
    }

    int high[6], low[6];
    for (int i = 0; i < 6; ++i) {
        high[i] = mDay[i].high;
        low[i] = mDay[i].low;
    }
    m_highChart->setData(high);
    m_lowChart->setData(low);

    m_cache->save(cityCode, mToday, mDay);
}


