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
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    void updataUI();
    void showDefaultUI();

private slots:
    void on_pushButton_2_clicked();
    void onWeatherDataReady(const Today &today, const Day day[6]);

private:
    void showCityDropdown();
    bool loadRecentCache(QString &outCityCode);
    void loadIconFont();
    QPixmap renderWeatherIcon(const QString &weather);

    Ui::Widget *ui;
    QMenu *mExitMenu;
    QAction *mExitAct;
    QAction *m_minimizeAct;
    QSystemTrayIcon *m_trayIcon;

    QPoint mOffset;

    Today mToday;
    Day mDay[6];

    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;
    QList<QLabel*> mAqiList;
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    QMap<QString, QChar> mIconCodeMap;
    QMap<QString, QPixmap> m_iconCache;
    QFont m_iconFont;

    ApiClient *m_apiClient;
    DataCache *m_cache;
    ChartWidget *m_highChart;
    ChartWidget *m_lowChart;

    QListWidget *m_cityDropdown;

    QString m_currentCityCode;
    bool m_startupCacheLoaded = false;
};
#endif
