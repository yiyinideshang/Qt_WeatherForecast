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
    //退出菜单项
    mExitMenu = new QMenu(this);
    mExitMenu->setStyleSheet("QMenu { color: black; } QMenu::item { color: black; }");

    //最小化到托盘菜单项
    m_minimizeAct = new QAction(QIcon("://res/MiniWindow.png"), "最小化到托盘");
    mExitMenu->addAction(m_minimizeAct);

    //添加分割线
    mExitMenu->addSeparator();

    //退出项
    mExitAct = new QAction();
    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));
    mExitMenu->addAction(mExitAct);

    connect(m_minimizeAct, &QAction::triggered, this, [this]() {
        //点击最小化到托盘项后，使主窗口隐藏，程序以后台方式运行，并使用户可以在系统托盘看到图标。
        hide();
        m_trayIcon->show();//该程序的系统托盘图标显示在系统托盘上
    });
    connect(mExitAct,&QAction::triggered,this,[=](){
    // qApp 是 Qt 框架预定义的一个全局宏，本质上等价于 QCoreApplication::instance()，
    // 返回指向当前应用程序唯一 QApplication 实例的指针。
    // exit(0) 会退出 Qt 的事件循环，导致应用程序完全终止。
    // 参数 0 是返回给操作系统的退出码，通常 0 表示正常退出。
    // 执行这一句后，窗口关闭、托盘图标消失，整个进程结束。
        qApp->exit(0);
    });

// 步骤 4：组织控件数组:将 UI 中的 6 组标签控件放入 QList，便于在 updataUI():将数据更新到 UI 控件中 用循环批量更新。
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


// 步骤 5：构建天气类型到图标的映射表   加载 iconfont 图标字体
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
    //1. 如果点击的是普通城市,优先使用cityCode，之后显示在lineEdit_2上；降级则使用cityName搜索
    //2. 如果点击的是被贴了"__clear__"的标签的"清空缓存"列表项,则执行清空缓存操作
    connect(m_cityDropdown, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        //如果点击的是 过滤暂无缓存记录
        if (!(item->flags() & Qt::ItemIsSelectable))//如果这个项没有 ItemIsSelectable 标记，就直接跳过，不处理点击。
            return;//作用就是过滤掉"暂无缓存记录"那行——它只有 ItemIsEnabled，没有 ItemIsSelectable，点了直接 return，啥也不干。

        //如果点击的是 清空缓存
        if (item->data(Qt::UserRole).toString() == "__clear__") {
            m_cache->clearAll();
            QSettings().remove("lastCityCode");
            ui->lineEdit_2->clear();//输入框显示的文本也清除掉
            m_cityDropdown->hide();//隐藏下拉框
            return;
        }

        //如果点击的是 某个城市
        m_cityDropdown->hide();

        // 优先使用 item 中存储的 cityCode（绕过 getCityCode 根据城市名字歧义判断）
        //static QString getCityCode(const QString &cityName)
        QString code = item->data(Qt::UserRole + 1).toString();
        if (!code.isEmpty()) {
            if (code == m_currentCityCode) return;//如果点击的还是当前城市,不做任何处理,直接返回

            //bool m_startupCacheLoaded = false;//标记启动时是否已展示缓存数据（控制错误弹窗文案）
            m_startupCacheLoaded = false;
            m_apiClient->cancelRetry();

            if (m_cache->load(code, mToday, mDay)) {//缓存命中,返回true
                m_lastRequestedCityCode = code;     //更新最近一次请求的城市的编码
                ui->lineEdit_2->setText(item->text());// 如果点击的是普通城市,则显示在lineEdit_2上
                onWeatherDataReady(mToday, mDay);//更新天气
                return;
            }
            //缓存未命中，说明这个城市从来没查过，或者缓存过期被清掉了。
            m_lastRequestedCityCode = code;//更新最近一次请求的城市的编码
            // 如果点击的是普通城市,则显示在lineEdit_2上
            ui->lineEdit_2->setText(item->text());
            m_apiClient->getWeatherInfo(code);//发起异步 HTTP 网络请求，成功后最终调用onWeatherDataReady
            return;
        }

        //技术兜底
        // 如果点击的是普通城市,则显示在lineEdit_2上
        ui->lineEdit_2->setText(item->text());
        // 降级：通过文字搜索
        on_pushButton_2_clicked();
    });

    // 向整个应用程序安装事件过滤器，让 this（当前 Widget 对象）成为全局事件监视者，
    // 之后整个应用程序的所有鼠标/键盘等事件都会先经过 Widget::eventFilter() 方法处理，
    // 之后才传给目标控件。
    qApp->installEventFilter(this);

    //禁止输入框的右键菜单（复制/粘贴等）。让输入框专门用于城市搜索，不显示标准右键菜单，保持界面简洁
    ui->lineEdit_2->setContextMenuPolicy(Qt::NoContextMenu);

    //当用户在输入框内按下回车键时，自动调用 on_pushButton_2_clicked() 执行搜索
    //这样用户输入城市名后无需点击按钮，直接回车即可查询天气。
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, this, &Widget::on_pushButton_2_clicked);

// 步骤 7：初始化网络客户端并连接信号
    //先进行 ApiClient 对象的初始化,这个类内部会创建QNetworkAccessManager网络管理器;
    //当 ApiClient::getWeatherInfo 发送http/Get请求 完毕触发QNetworkAccessManager::finished信号
    // 这个信号与onReplied 槽函数绑定,这个槽函数用于 处理服务器返回的数据,看看相应是否成功
    // 成功后调用 ApiClient::parseJson :这个函数用于 解析 JSON
    // 成功后:emit weatherDataReady 信号；weatherDataReady信号又会触发onWeatherDataReady进行处理
    // 失败后:emit errorOccurred("请求数据失败，请检查城市编码或网络连接~~！")

    m_apiClient = new ApiClient(this);
    //当ApiClient::weatherDataReady信号就绪时,槽函数Widget::onWeatherDataReady进行处理
    connect(m_apiClient, &ApiClient::weatherDataReady,
            this, &Widget::onWeatherDataReady);

    //失败信号errorOccurred携带的参数:失败信息
    connect(m_apiClient, &ApiClient::errorOccurred, this, [this](const QString &msg) {
        //bool m_startupCacheLoaded = false;//标记启动时是否已展示缓存数据（控制错误弹窗文案）
        //如果有缓存信息(步骤9),m_startupCacheLoaded设置为true,表示显示缓存
        if (m_startupCacheLoaded) {
            m_startupCacheLoaded = false;   //重置标识
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("天气");
            msgBox.setText("网络连接失败，当前显示缓存数据，正在后台重连...");//有缓存时,信息框弹出这个
            msgBox.setStyleSheet("color: black;");
            msgBox.exec();
        } else {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("天气");
            // 当请求数据失败时(网络断开):请求数据失败，请检查网络连接~~！
            // 当输入的城市找不到城市编码时:请检查输入的城市是否正确!(省级以下的城市)
            msgBox.setText(msg);
            msgBox.setStyleSheet("color: black;");
            msgBox.exec();
        }
    });

// 步骤 8：初始化本地缓存层
    // DataCache *m_cache;//SQLite 本地缓存
    m_cache = new DataCache(this);

// 步骤 9：立即显示 UI + 后台网络请求
    //存的是一个字符串（城市编码），保存在注册表 / 配置文件里，用来记录上次访问的城市。作用：下次启动时直接读这个编码，不走 IP 定位。
    QSettings settings;//记"上次用哪个城市"
    QString lastCityCode = settings.value("lastCityCode").toString();

    //1. 先试 QSettings 记的那个城市。 如果 lastCityCode 非空，就去数据库里读这个城市的缓存——读到的结果给到mToday, mDay,并返回 true，进入 if。
    //2. 第 1 条失败时降级。去数据库里找最近缓存过的城市（不管 lastCityCode 是什么），读到了就 true。
    if ((!lastCityCode.isEmpty() && m_cache->load(lastCityCode, mToday, mDay)) ||
        loadRecentCache(lastCityCode)) {
        updataUI();//将数据更新到 UI 控件
        m_currentCityCode = lastCityCode;//记录当前加载的城市编码，用于后续搜索时去重。
        m_startupCacheLoaded = true;//标志控制首次失败时的弹窗文案;// 它是一个标记：已经用缓存显示了
    } else
        showDefaultUI();// 完全没缓存 → 显示占位符

    m_lastRequestedCityCode = lastCityCode;

// 将网络请求推迟到事件循环启动后执行，避免 `m_highChart`/`m_lowChart` 尚未初始化就被访问。
// 0 是延迟 0 毫秒，但 Qt 不会真的等 0ms——它把这个任务扔到事件队列的末尾，等当前函数（构造函数）执行完毕、控件都显示出来后，再执行里面的代码。
    QTimer::singleShot(0, this, [this, lastCityCode]() {
        if (!lastCityCode.isEmpty())// 有上次城市 → 直接用那个城市发起get请求

            //当 ApiClient::getWeatherInfo 发送http/Get请求 完毕触发QNetworkAccessManager::finished信号
            // 这个信号与onReplied 槽函数绑定,这个槽函数用于 处理服务器返回的数据,看看相应是否成功，
            // 成功后调用 ApiClient::parseJson :这个函数用于 解析 JSON 并发射 weatherDataReady 信号
            //（步骤7）当ApiClient::weatherDataReady信号就绪时,槽函数Widget::onWeatherDataReady进行处理
            m_apiClient->getWeatherInfo(lastCityCode);
        else
            // 当ApiClient::getLocationByIP 发送http/Get请求 完毕触发QNetworkAccessManager::finished信号
            // 先在getLocationByIP函数内部进行处理，之后再交给getWeatherInfo 再次发送http/Get请求
            m_apiClient->getLocationByIP();// 没有上次城市 → 走 IP 定位来发起get请求
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

    //① "显示"菜单项
    connect(showAct, &QAction::triggered, this, [this]() {
        // 使用 showNormal() 而非 show()，确保任务栏最小化的窗口也能正确恢复。
        showNormal(); // 从最小化/隐藏状态恢复窗口

        //强制窗口置顶，避免藏在其他窗口后面。
        raise();// 把窗口提到最前面
        activateWindow();// 给窗口焦点（激活）
    });

    //② "退出"菜单项
    connect(trayExitAct, &QAction::triggered, this, [this]() {
        m_trayIcon->hide();// 先隐藏托盘图标
        qApp->exit(0);// 退出应用
    });

    // ③ 托盘图标点击
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        //鼠标单击或双击托盘图标时恢复窗口。ActivationReason 参数区分点击方式（单击 / 双击 / 中键等），只对单击和双击响应，忽略其他操作。
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            //与上方显示菜单项一致
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


//加载 iconfont.ttf + 解析 iconfont.json，构建 mIconCodeMap
void Widget::loadIconFont()
{
    // 1.加载 .ttf 字体文件
    int fontId = QFontDatabase::addApplicationFont(":/res/weatherforecast_icon/iconfont.ttf");
    if (fontId >= 0) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty())
            //QFont m_iconFont;
            m_iconFont = QFont(families.first(), 40);
    }

    // 2. 打开并检查JSON文件
    QFile f(":/res/weatherforecast_icon/iconfont.json");
    if (!f.open(QIODevice::ReadOnly))//以只读模式尝试打开这个文件。
        return;//打开失败立即退出
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());//读取并解析成JSON数据
    f.close();//关闭JSON文件
    if (!doc.isObject())//根据iconfont.json文件可知,它的最外层是一个{}JSON对象,如果不是可能是因为:文件是空的、被篡改了
        return;//返回JSON对象失败立即返回

    // 3. 解析 .json 文件，建立映射表
    QJsonArray glyphs = doc.object().value("glyphs").toArray();
    for (const QJsonValue &v : glyphs) {
        QJsonObject g = v.toObject();
        QString name = g.value("name").toString();      // 比如取到 "dayu"
        QString unicodeHex = g.value("unicode").toString(); // 比如取到 "e678"
        //注意，这里的 unicode 存的是十六进制字符串（不带 0x 前缀），比如 "e678"，而不是真正的数字。

        bool ok;//ok 是一个布尔引用，用来告诉你转换是否成功
        uint code = unicodeHex.toUInt(&ok, 16);//把字符串 "e678" 当成 16 进制数，转换成无符号整数 0xE695

        // QMap<QString, QChar> mIconCodeMap;
        if (ok && !name.isEmpty())//只插入合法数据：只有当转换成功（ok == true）并且名字不为空时，才插入到 QMap 中。
            mIconCodeMap.insert(name, QChar(code));//把整数 e678 强制转成 Qt 的字符类型 QChar
        // 此时的 mIconCodeMap长这样：
        // Key("dayu")    -> Value(QChar(0xE678))
        // Key("duoyun")  -> Value(QChar(0xE67f))
    }

    // 4. 人工处理别名
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

//QPainter 将天气名称渲染为 QPixmap（iconfont），结果缓存到 m_iconCache
//QMap<QString, QPixmap> m_iconCache;//QPixmap 图标缓存（iconfont 渲染结果），避免重复 QPainter 绘制
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

// 重写事件：记录拖拽偏移
void Widget::mousePressEvent(QMouseEvent *event)
{
    //event->globalPos() 是鼠标在屏幕上的全局坐标。
    //this->pos() 是当前窗口左上角在屏幕上的全局坐标
    //两者相减，得到鼠标点击位置到窗口左上角的向量，保存在成员变量 mOffset 中。
    //鼠标到窗口左上角偏移量 mOffset
    // Qt6 兼容写法
    mOffset = event->globalPosition().toPoint() - this->pos();
}
// 重写事件：实现无边框窗口拖拽
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //event->globalPos() 得到鼠标移动后的新坐标
    //此时窗口应该在:该鼠标新位置-鼠标到窗口左上角那个固定的偏移量
    this->move(event->globalPosition().toPoint() - mOffset);
}

//将数据更新到 UI 控件
void Widget::updataUI()
{
    //1. 更新日期和城市
    QDate today = QDate::currentDate();
    QLocale locale("zh_CN");
    ui->lblDate->setText(
        QString("<p><span style='font-size:16pt; font-weight:700;'>%1 %2</span></p>")
            .arg(today.toString("yyyy/MM/dd"), locale.dayName(today.dayOfWeek()))
        );
    {
        int pt = 16;
        QFont f = ui->lblCity->font();
        f.setPointSize(pt);
        QFontMetrics fm(f);
        int tw = fm.horizontalAdvance(mToday.city);
        int mw = ui->lblCity->width() - 12;
        while (tw > mw && pt > 8) {
            f.setPointSize(--pt);
            fm = QFontMetrics(f);
            tw = fm.horizontalAdvance(mToday.city);
        }
        ui->lblCity->setText(
            QString("<p align='center'><span style='font-size:%1pt;'>%2</span></p>")
                .arg(pt).arg(mToday.city)
            );
    }

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

//设置占位符文本（不依赖数据模型默认值）
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

//重写监听事件
// 目的是：在下拉框 m_cityDropdown 的显示与隐藏，
// 检测用户是否点击了下拉框和触发输入框之外的区域，若是则自动隐藏下拉框（在 eventFilter 中实现）。
// 同时它也负责在点击输入框时切换下拉的显隐。
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

//重写 changeEvent，确保在窗口最小化时自动隐藏下拉框。
void Widget::changeEvent(QEvent *event)
{
    // 1. 判断事件类型是否是"窗口状态变化"(只关心窗口状态变化（最小化/最大化/还原/全屏)
    if (event->type() == QEvent::WindowStateChange) {
        // 2. 窗口正在最小化 && 下拉框还在显示
        if (isMinimized() && m_cityDropdown->isVisible())
            m_cityDropdown->hide();  // 3. 强制隐藏下拉框
        //如果下拉框本来就没显示，就不需要多此一举去 hide()。
    }
    // 4. 调用基类实现
    QWidget::changeEvent(event);//如果不调用父类的 changeEvent，Qt 的默认窗口状态处理逻辑可能被跳过，导致最小化/最大化等行为异常。
}

//从缓存读取城市列表，弹出下拉框
void Widget::showCityDropdown()
{
    m_cityDropdown->clear();
    // 获取所有历史城市,之后遍历它,获得下拉列表框中的缓存城市
    auto cities = m_cache->getAllCachedCities();

    //如果下拉列表框是空的,则只有一个"暂无缓存记录"列表项
    if (cities.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem("暂无缓存记录");
        emptyItem->setFlags(Qt::ItemIsEnabled);//只保留可显示
        emptyItem->setForeground(QColor(160, 160, 160));//灰色
        m_cityDropdown->addItem(emptyItem);//添加列表项
    }
    else {
        // DisplayRole="北京", UserRole+1="101010100"
        // DisplayRole="上海", UserRole+1="101020100"
        // DisplayRole="清除缓存记录", UserRole="__clear__"
        for (const auto &pair : qAsConst(cities)) {
            //flags为可选中 + 可显示
            auto *item = new QListWidgetItem(pair.second);// DisplayRole = 城市名
            item->setData(Qt::UserRole + 1, pair.first);// UserRole+1 = cityCode
            m_cityDropdown->addItem(item);
        }

        //最后添加一个列表项:"清除缓存记录"
        QListWidgetItem *clearItem = new QListWidgetItem("清除缓存记录");
        clearItem->setData(Qt::UserRole, "__clear__");
        clearItem->setForeground(QColor(160, 160, 160));//设置该项的文字颜色为浅灰色（RGB 各通道都是 160，即中灰色）。
        m_cityDropdown->addItem(clearItem);//将 clearItem 这个已经构造好的列表项添加到 m_cityDropdown 这个下拉列表中。
    }

    // 1. 将输入框左下角的局部坐标转换为全局屏幕坐标
    QPoint pos = ui->lineEdit_2->mapToGlobal(QPoint(0, ui->lineEdit_2->height()));
    m_cityDropdown->setFixedWidth(ui->lineEdit_2->width());
    m_cityDropdown->move(pos);
    m_cityDropdown->show();
}

//lastCityCode 缓存未命中时的降级：取最近缓存城市
bool Widget::loadRecentCache(QString &outCityCode)
{
    auto cities = m_cache->getAllCachedCities();// 取所有缓存城市
    if (cities.isEmpty())
        return false;

    outCityCode = cities.first().first;// 取最新的一条（按更新时间排序）
    return m_cache->load(outCityCode, mToday, mDay);// 读它的数据给到mToday, mDay
}

//搜索按钮点击 / Enter 键响应
void Widget::on_pushButton_2_clicked()
{
    QString cityName = ui->lineEdit_2->text();
    QString cityCode = WeatherTool::getCityCode(cityName);//通过WeatherTool类的getCityCode得到城市编码

    //加上!cityCode.isEmpty()，强制界面存在城市且输入的城市就是当前城市时，直接返回
    //当cityCode.isEmpty()表示当前无缓存
    //当无网络时，界面城市信息为空，当前城市也是空；如果不加上这个条件，当输入不存在的城市后，啥错误信息没返回，
    // 应该返回：请检查输入的城市是否正确!(省级以下的城市)

    // 只有当 当前UI中有城市，并且搜索的城市与当前UI中的城市相同,保持不变,搜索按钮直接返回
    if (!cityCode.isEmpty() && cityCode == m_currentCityCode)
        return;

    //搜索时让显示缓存标识为false,确保当搜索失败后的提示为:请求数据失败，请检查城市编码或网络连
    m_startupCacheLoaded = false;

    if (!cityCode.isEmpty()) {
        //搜索时,停掉之前的重试定时器,重置计数，新请求重新从 0 开始
        m_apiClient->cancelRetry();
        QSettings settings;
        settings.setValue("lastCityCode", cityCode);

        if (m_cache->load(cityCode, mToday, mDay)) {
            m_lastRequestedCityCode = cityCode;
            onWeatherDataReady(mToday, mDay);
            return;
        }
        m_lastRequestedCityCode = cityCode;
        m_apiClient->getWeatherInfo(cityCode);
        return;
    }

    // cityCode 为空：区分"重名歧义"和"未找到"
    if (!cityName.contains('(')) {
        QStringList opts = WeatherTool::getAmbiguousOptions(cityName);
        if (!opts.isEmpty()) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(QString("找到多个同名城市，请准确输入：\n%1").arg(opts.join("、")));
            msgBox.exec();
            return;
        }
    }

    m_apiClient->getWeatherInfo(cityName);
}

//今日天气数据 和 7日天气数据
void Widget::onWeatherDataReady(const Today &today, const Day day[7])
{
    mToday = today;
    for (int i = 0; i < 7; ++i) mDay[i] = day[i];
    updataUI();

    QSettings settings;
    QString cityCode = WeatherTool::getCityCode(mToday.city);
    if (cityCode.isEmpty())
        cityCode = m_lastRequestedCityCode;
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

    if (!cityCode.isEmpty())
        m_cache->save(cityCode, mToday, mDay);
}


