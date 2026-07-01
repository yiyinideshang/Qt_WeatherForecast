#include "apiclient.h"

#include <QString>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include "weatherTool.h"

ApiClient::ApiClient(QObject *parent)
    : QObject{parent}
{
    mNetAccessManger = new QNetworkAccessManager(this);
    connect(mNetAccessManger,&QNetworkAccessManager::finished,this,&ApiClient::onReplied);

    m_retryTimer = new QTimer(this);
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &ApiClient::retry);
}

void ApiClient::cancelRetry()
{
    m_retryTimer->stop();
    m_retryCount = 0;
}

void ApiClient::getWeatherInfo(const QString &cityName)
{
    cancelRetry();

    QString cityCode = cityName;

    if (cityName.startsWith("101") && cityName.length() == 9) {
        qDebug() << "直接使用城市编码:" << cityCode;
    } else {
        cityCode = WeatherTool::getCityCode(cityName);
        if(cityCode.isEmpty()){
            emit errorOccurred("请检查输入的城市是否正确!(省级以下的城市)");
            return;
        }
    }

    m_pendingCityCode = cityCode;

    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
    mNetAccessManger->get(QNetworkRequest(url));
}

void ApiClient::getLocationByIP()
{
    auto *ipManager = new QNetworkAccessManager(this);
    connect(ipManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        reply->deleteLater();
        ipManager->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            getWeatherInfo("北京");
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QString cityPinyin = doc.object().value("city").toString();
        QString cityCode = WeatherTool::getCityCodeByPinyin(cityPinyin);
        qDebug() << "自动定位到:" << cityPinyin << "code:" << cityCode;
        getWeatherInfo(cityCode.isEmpty() ? "北京" : cityCode);
    });
    ipManager->get(QNetworkRequest(QUrl("https://ipinfo.io/json")));
}


//槽函数用于处理服务器返回的数据
void ApiClient::onReplied(QNetworkReply *reply)
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
        if (m_retryCount == 0) {
            emit errorOccurred("请求数据失败，请检查城市编码或网络连接~~！");
        }
        if (++m_retryCount <= MAX_RETRY_COUNT) {
            qDebug() << "后台重试" << m_retryCount << "/" << MAX_RETRY_COUNT;
            m_retryTimer->start(RETRY_INTERVAL_MS);
        }
    }
    else
    {
        cancelRetry();
        //获取响应信息
        //用 reply->readAll() 读取服务器返回的完整数据（这里就是天气的 JSON 字符串），并打印出来。
        QByteArray byteArray = reply->readAll();
        qDebug()<<"read all:"<<byteArray.data();
        parseJson(byteArray);
    }
    //最后必须调用 reply->deleteLater() 释放网络回复对象的内存，避免内存泄漏。
    reply->deleteLater();
}

void ApiClient::parseJson(const QByteArray &byteArray)
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
        emit errorOccurred("请求数据失败，请检查城市编码或网络连接~~！");
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
    emit weatherDataReady(mToday, mDay);
}

void ApiClient::retry()
{
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + m_pendingCityCode);
    mNetAccessManger->get(QNetworkRequest(url));
}
