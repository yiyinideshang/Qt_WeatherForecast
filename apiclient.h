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

    void getWeatherInfo(const QString &cityName);//根据城市名/编码发起 GET 请求，先 cancelRetry()
    void getLocationByIP();//通过 ipinfo.io 自动定位
    void cancelRetry();//停止重试定时器，重置计数（getWeatherInfo 开头调用）
signals:
    void weatherDataReady(const Today &today, const Day day[7]);//数据就绪后通知 Widget
    void errorOccurred(const QString &message);//第一次失败时立即发射（弹窗提示用户），后续静默重试
private slots:
    void onReplied(QNetworkReply *reply);//处理 HTTP 响应，失败时启动定时器重试，成功则调用 parseJson
    void retry();//使用 m_pendingCityCode 重新发送 GET 请求（定时器触发）
private:
    void parseJson(const QByteArray &byteArray);//解析 JSON 并发射 weatherDataReady 信号

    QNetworkAccessManager *mNetAccessManger;//网络管理对象
    QTimer *m_retryTimer;//重试定时器（单次触发模式）

    Today mToday;//今日天气数据（解析结果）
    Day mDay[7];// 7天天气数据

    int m_retryCount = 0;//当前请求已重试次数
    QString m_pendingCityCode;//正在请求的城市编码（重试时复用）
    static constexpr int RETRY_INTERVAL_MS = 30000;//重试间隔（30000ms = 30 秒）
    static constexpr int MAX_RETRY_COUNT = 10;//最大重试次数（10）
};

#endif // APICLIENT_H
