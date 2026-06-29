#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QContextMenuEvent>

//它相当于一个网络管家，管理着所有网络请求的发送与接收
#include <QNetworkAccessManager>
// 它代表服务器返回的应答，是一个具体的响应对象，继承自 QIODevice，所以你可以像读文件一样读取响应数据。
#include <QNetworkReply>

#include "weatherdata.h"
#include <QList>
#include <QLabel>

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
    //重写父类的方法
    //捕获用户在窗口上点击鼠标右键（或触控板双指点击）的事件，以便弹出自定义的菜单（右键菜单/上下文菜单）。
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    //声明一个用于发送http请求的成员,来获取天气数据
    void getWeatherInfo(QString cityCode);

    //解析JSON
    void parseJson(QByteArray &byteArray);

    //更新UI
    void updataUI();

    //(2)重写父类的 eventsfilter 方法
    bool eventFilter(QObject* watched,QEvent* event) override;

    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();
private slots:
    void onReplied(QNetworkReply *reply);
    void on_pushButton_2_clicked();
    void getLocationByIP();

private:
    Ui::Widget *ui;
    QMenu *mExitMenu;   //右键退出的菜单
    QAction *mExitAct;  //右键退出的菜单项(退出的行为)

    //窗口移动时,鼠标与窗口左上角的偏移
    QPoint mOffset;

    //http请求,//它相当于一个网络管家，管理着所有网络请求的发送与接收
    QNetworkAccessManager *mNetAccessManger;

    //今天和6天的信息(昨天/今天/明天/后三天)
    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;
    //天气和天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;
    //天气污染指数
    QList<QLabel*> mAqiList;
    //风力和风向
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    //
    QMap<QString,QString> mTypeMap;

};
#endif // WIDGET_H
