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

// 步骤 1：初始化成员列表与 UI 设置
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //解析 widget.ui，动态创建所有子控件，并建立信号-槽自动连接。
    ui->setupUi(this);

    // 加上这一句：强制让 QWidget 遵循样式表去绘制背景
    this->setAttribute(Qt::WA_StyledBackground, true);

// 步骤 2：设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint); //设置窗口无边框
    setFixedSize(width(),height());         //设置固定窗口尺寸为 1000x600
    setWindowIcon(QIcon("://res/WeatherForecast.png")); //设置任务栏和窗口图标

// 步骤 3：构建右键菜单（最小化到托盘 + 退出）
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

// 步骤 4：组织控件数组:将 UI 中的 6 组标签控件放入 QList，便于在 updataUI() 中用循环批量更新。
    //星期和日期
    mWeekList<<ui->lblWeek0<<ui->lblWeek1<<ui->lblWeek2<<ui->lblWeek3<<ui->lblWeek4<<ui->lblWeek5<<ui->lblWeek6;
    mDateList<<ui->lblDate0<<ui->lblDate1<<ui->lblDate2<<ui->lblDate3<<ui->lblDate4<<ui->lblDate5<<ui->lblDate6;
    //天气和天气图标
    mTypeList<<ui->lblType0<<ui->lblType1<<ui->lblType2<<ui->lblType3<<ui->lblType4<<ui->lblType5<<ui->lblType6;
    mTypeIconList<<ui->lblTypeIcon0<<ui->lblTypeIcon1<<ui->lblTypeIcon2<<ui->lblTypeIcon3<<ui->lblTypeIcon4<<ui->lblTypeIcon5<<ui->lblTypeIcon6;
    //天气指数
    mAqiList<<ui->lblquality0<<ui->lblquality1<<ui->lblquality2<<ui->lblquality3<<ui->lblquality4<<ui->lblquality5<<ui->lblquality6;
    //风力和风向
    mFxList<<ui->lblFX0<<ui->lblFX1<<ui->lblFX2<<ui->lblFX3<<ui->lblFX4<<ui->lblFX5<<ui->lblFX6;
    mFlList<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<<ui->lblFl4<<ui->lblFl5<<ui->lblFl6;

// 步骤 5：构建天气类型到图标的映射表
    loadIconFont();

// 步骤 6：创建搜索框下拉列表（最近访问城市）
    m_cityDropdown = new QListWidget;
    m_cityDropdown->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);//工具窗口|无边框窗口
    m_cityDropdown->setFocusPolicy(Qt::NoFocus);
    m_cityDropdown->setFrameShape(QFrame::NoFrame);
    m_cityDropdown->setStyleSheet(
        "QListWidget { background: white; border: 1px solid #ccc; color: black; font-size: 12pt; }"
        "QListWidget::item { padding: 6px 12px; }"
        "QListWidget::item:hover { background: #e0e0e0; }"
    );
    m_cityDropdown->hide();//创建完毕一个空的QListWidget下拉列表后:主动隐藏下拉框

    //当用户点击列表中的某一项时，通过信号itemClicked把这个被点击的项传递给lambda槽函数进行处理
    //这个信号携带item参数,表示被点击的是具体哪一个列表项
    //在这个lambda槽函数里面进行判断,
    //1. 如果点击的是普通城市,则显示在lineEdit_2上,并搜索该城市的信息,
    //2. 如果点击的是被贴了"__clear__"的标签的"清空缓存"列表项,则执行清空缓存操作
    connect(m_cityDropdown, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        // 先检查项是否可选择（如果是分隔符等不可选项，直接返回）
        if (!(item->flags() & Qt::ItemIsSelectable))
            return;

        // 点击时读取标签：如果点击的该项在指定为“UserRole”下的数据是“__clear__”
        // 则通过 UserRole 数据区分“清空缓存”特殊项
        if (item->data(Qt::UserRole).toString() == "__clear__") {
            m_cache->clearAll();//清除缓存信息
            QSettings().remove("lastCityCode");//删除持久化存储中的“上次城市代码”记录。
            ui->lineEdit_2->clear();//清除lineEdit_2的文本信息
            m_cityDropdown->hide();//清空缓存后,隐藏下拉框
            return;
        }

        //否则,点击的是一个普通的城市项
        //将点击的列表项输入到 lineEdit_2 上,并调用on_pushButton_2_clicked槽函数,完成搜索功能
        m_cityDropdown->hide();//用户选择了某个城市后,隐藏下拉框
        ui->lineEdit_2->setText(item->text());
        on_pushButton_2_clicked();
    });
    qApp->installEventFilter(this);

    ui->lineEdit_2->setContextMenuPolicy(Qt::NoContextMenu);
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, this, &Widget::on_pushButton_2_clicked);

// 步骤 7：初始化网络客户端并连接信号
    //网络请求
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

// 步骤 8：初始化本地缓存层
    m_cache = new DataCache(this);

// 步骤 9：立即显示 UI + 后台网络请求
    QSettings settings;
    QString lastCityCode = settings.value("lastCityCode").toString();

    if ((!lastCityCode.isEmpty() && m_cache->load(lastCityCode, mToday, mDay)) ||
        loadRecentCache(lastCityCode)) {
        updataUI();
        m_currentCityCode = lastCityCode;//记录当前加载的城市编码，用于后续搜索时去重。
        m_startupCacheLoaded = true;//标志控制首次失败时的弹窗文案
    } else
        showDefaultUI();

// 将网络请求推迟到事件循环启动后执行，避免 `m_highChart`/`m_lowChart` 尚未初始化就被访问。
    QTimer::singleShot(0, this, [this, lastCityCode]() {
        if (!lastCityCode.isEmpty())
            m_apiClient->getWeatherInfo(lastCityCode);
        else
            m_apiClient->getLocationByIP();
    });

//步骤 10：创建温度曲线控件
    ui->lblGanmao->setWordWrap(true);

    //创建两个 ChartWidget 实例分别绘制高温（橙色）和低温（青色）曲线。
    m_highChart = new ChartWidget(QColor(255, 170, 0), ui->widget_5);
    m_highChart->setGeometry(0, 0, 460, 81);
    m_lowChart = new ChartWidget(QColor(0, 255, 255), ui->widget_5);
    m_lowChart->setGeometry(0, 70, 470, 81);

// 步骤 11：窗口位置恢复
    QSettings s;
    QPoint pos = s.value("windowPos").toPoint();
    if (!pos.isNull()) {
        QTimer::singleShot(50, this, [this, pos]() {
            QScreen *screen = QGuiApplication::primaryScreen();
            if (screen && screen->availableGeometry().contains(pos))
                move(pos);
        });
    }

// 步骤 12：退出时保存窗口位置
    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        QSettings s;
        s.setValue("windowPos", this->pos());
    });

// 步骤 13：系统托盘初始化
    m_trayIcon = new QSystemTrayIcon(QIcon("://res/WeatherForecast.png"), this);
    m_trayIcon->setToolTip("WeatherForecast");

    QMenu *trayMenu = new QMenu(this);
    trayMenu->setStyleSheet("QMenu { color: black; } QMenu::item { color: black; }");
    QAction *showAct = trayMenu->addAction("显示");
    QAction *trayExitAct = trayMenu->addAction("退出");
    m_trayIcon->setContextMenu(trayMenu);

    connect(showAct, &QAction::triggered, this, [this]() {
        showNormal();//使用 showNormal() 而非 show()，确保任务栏最小化的窗口也能正确恢复。
        //- 强制窗口置顶，避免藏在其他窗口后面。
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

    //3. 更新七天
    for(int i = 0;i<7;i++){
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

    for (int i = 0; i < 7; i++) {
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
    // 获取所有历史城市,之后遍历它,获得下拉列表框中的缓存城市
    auto cities = m_cache->getAllCachedCities();


    //如果下拉列表框是空的,则只有一个"暂无缓存记录"列表项
    if (cities.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无缓存记录");
        emptyItem->setFlags(Qt::ItemIsEnabled);//项处于启用状态
        emptyItem->setForeground(QColor(160, 160, 160));//灰色
        m_cityDropdown->addItem(emptyItem);//添加列表项
    }
    else {
        for (const auto &pair : qAsConst(cities))//在这里遍历
            m_cityDropdown->addItem(pair.second);//将历史城市缓存一个一个添加到下拉列表框中

        //最后添加一个列表项:"清除缓存记录"
        QListWidgetItem *clearItem = new QListWidgetItem("清除缓存记录");
        clearItem->setData(Qt::UserRole, "__clear__");
        clearItem->setForeground(QColor(160, 160, 160));//设置该项的文字颜色为浅灰色（RGB 各通道都是 160，即中灰色）。
        m_cityDropdown->addItem(clearItem);//将 clearItem 这个已经构造好的列表项添加到 m_cityDropdown 这个下拉列表中。
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

void Widget::onWeatherDataReady(const Today &today, const Day day[7])
{
    mToday = today;
    for (int i = 0; i < 7; ++i) mDay[i] = day[i];
    updataUI();

    QSettings settings;
    QString cityCode = WeatherTool::getCityCode(mToday.city);
    if (!cityCode.isEmpty()) {
        settings.setValue("lastCityCode", cityCode);
        m_currentCityCode = cityCode;
    }

    int high[7], low[7];
    for (int i = 0; i < 7; ++i) {
        high[i] = mDay[i].high;
        low[i] = mDay[i].low;
    }
    m_highChart->setData(high);
    m_lowChart->setData(low);

    m_cache->save(cityCode, mToday, mDay);
}


