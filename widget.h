#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QContextMenuEvent>
#include <QSystemTrayIcon>

#include "weatherdata.h"
#include <QList>
#include <QLabel>
#include <QMap>
#include <QPixmap>
#include <QListWidget>
#include <QFont>
#include "apiclient.h"
#include "chartwidget.h"
#include "datacache.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    //重写事件:
    void contextMenuEvent(QContextMenuEvent *event) override;  // 右键弹出退出/最小化到托盘菜单
    void mousePressEvent(QMouseEvent *event) override;         // 记录拖拽偏移
    void mouseMoveEvent(QMouseEvent *event) override;          // 实现无边框窗口拖拽
    bool eventFilter(QObject *obj, QEvent *event) override;    //监听全局鼠标点击关闭 dropdown；

    //重写 changeEvent，确保在窗口最小化时自动隐藏下拉框。
    void changeEvent(QEvent *event) override;

    void updataUI();//将数据更新到 UI 控件
    void showDefaultUI();//设置占位符文本（不依赖数据模型默认值）

private slots:
    void on_pushButton_2_clicked();//搜索按钮点击 / Enter 键响应
    void onWeatherDataReady(const Today &today, const Day day[7]);//接收 API 数据并更新 UI 和曲线

private:
    void showCityDropdown();//从缓存读取城市列表，弹出下拉框
    bool loadRecentCache(QString &outCityCode);//lastCityCode 缓存未命中时的降级：取最近缓存城市
    void loadIconFont();//加载 iconfont.ttf + 解析 iconfont.json，构建 mIconCodeMap
    //QPainter 将天气名称渲染为 QPixmap（iconfont），结果缓存到 m_iconCache
    QPixmap renderWeatherIcon(const QString &weather);

    Ui::Widget *ui; //UI 对象指针
    QMenu *mExitMenu; //右键菜单（最小化到托盘 / 退出）
    QAction *mExitAct; //退出菜单项
    QAction *m_minimizeAct; //最小化到托盘菜单项
    QSystemTrayIcon *m_trayIcon;//系统托盘图标

    QPoint mOffset;//鼠标拖拽偏移量

    Today mToday;//今日天气数据
    Day mDay[7];//7 天天气数据

    QList<QLabel*> mWeekList;//星期标签集合
    QList<QLabel*> mDateList;//日期标签集合
    QList<QLabel*> mTypeList;//天气类型标签集合
    QList<QLabel*> mTypeIconList;//天气图标标签集合
    QList<QLabel*> mAqiList;//AQI 标签集合
    QList<QLabel*> mFxList;//风向标签集合（lblFX0~6, row 0）
    QList<QLabel*> mFlList;//风力标签集合（lblFl0~6, row 1）

    QMap<QString, QChar> mIconCodeMap;//iconfont glyph 名称到 Unicode 码点的映射
    QMap<QString, QPixmap> m_iconCache;//QPixmap 图标缓存（iconfont 渲染结果），避免重复 QPainter 绘制
    QFont m_iconFont;//iconfont 字体对象，由 loadIconFont() 初始化

    ApiClient *m_apiClient;//网络 API 客户端
    DataCache *m_cache;//SQLite 本地缓存层
    ChartWidget *m_highChart;//高温曲线控件
    ChartWidget *m_lowChart;//低温曲线控件

    QListWidget *m_cityDropdown;//最近访问城市下拉列表

    QString m_currentCityCode = "";//当前显示的城市的编码（用于搜索去重）
    QString m_lastRequestedCityCode;//最近一次请求的城市编码（onWeatherDataReady 歧义回退）
    bool m_startupCacheLoaded = false;//标记启动时是否已展示缓存数据（控制错误弹窗文案）
    //当它为true时:失败提示为:"网络连接失败，当前显示缓存数据..."（温和提示）
    //当它为false时:失败提示为:"请求数据失败，请检查网络连接~~！"（错误警告）
};
#endif
