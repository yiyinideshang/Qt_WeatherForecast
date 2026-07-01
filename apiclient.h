#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>

//它相当于一个网络管家，管理着所有网络请求的发送与接收
#include <QNetworkAccessManager>
// 它代表服务器返回的应答，是一个具体的响应对象，继承自 QIODevice，你可以像读文件一样读取响应数据。
#include <QNetworkReply>
#include <QTimer>
#include "weatherdata.h"

class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void getWeatherInfo(const QString &cityName);
    void getLocationByIP();
    void cancelRetry();
signals:
    void weatherDataReady(const Today &today, const Day day[6]);
    void errorOccurred(const QString &message);
private slots:
    void onReplied(QNetworkReply *reply);
    void retry();
private:
    void parseJson(const QByteArray &byteArray);

    QNetworkAccessManager *mNetAccessManger;
    QTimer *m_retryTimer;

    Today mToday;
    Day mDay[6];

    int m_retryCount = 0;
    QString m_pendingCityCode;
    static constexpr int RETRY_INTERVAL_MS = 30000;
    static constexpr int MAX_RETRY_COUNT = 10;
};

#endif // APICLIENT_H
