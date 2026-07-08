#include "apiclient.h"

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QtMath>
#include "weatherTool.h"

ApiClient::ApiClient(QObject *parent)
    : QObject{parent}
{
    mNetAccessManger = new QNetworkAccessManager(this); // ① 创建网络管理器
    //网络请求
    //关联信号和槽,当ApiClient::getWeatherInfo 发送http/Get请求 完毕后,服务器返回数据,此时:
    //QNetworkAccessManager::mNetAccessManger 就会发送一个finished信号,
    // 并将QNetworkReply的指针携带服务器的响应,作为参数传递出去(传递给 onReplied 槽函数),
    //进而调用onReplied槽函数作出 处理响应动作
    connect(mNetAccessManger,&QNetworkAccessManager::finished,this,&ApiClient::onReplied);// ② 连接信号与槽

    m_retryTimer = new QTimer(this);//重试定时器（单次触发模式）
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &ApiClient::retry);//使用 m_pendingCityCode 重新发送 GET 请求（定时器触发）
}

//停止重试定时器，重置计数（getWeatherInfo 开头、onReplied调用）
void ApiClient::cancelRetry()
{
    m_retryTimer->stop();// 停掉之前的重试定时器
    m_retryCount = 0;// 重置计数，新请求重新从 0 开始
}

//根据城市名/编码发起 GET 请求，先 cancelRetry()
void ApiClient::getWeatherInfo(const QString &cityName)
{
    QString cityCode = cityName;

    if (cityName.startsWith("101") && cityName.length() == 9) {
        qDebug() << "直接使用城市编码:" << cityCode;
    } else {
        //WeatherTool::getCityCode:根据中文名查编码，支持补/去 "市" 后缀，支持补/去 "县" 后缀
        cityCode = WeatherTool::getCityCode(cityName);
        if(cityCode.isEmpty()){
            emit errorOccurred("请检查输入的城市是否正确!(省级以下的城市)");
            return;     // ← 当输入的城市有误时（本地json文件没有该城市），这里直接 return，根本不会走到 onReplied
        }
    }

    cancelRetry();
    m_pendingCityCode = cityCode;

    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);

    // 发出 HTTP GET 请求
    // get 内部会创建一个 QNetworkReply 对象，启动 HTTP 请求,服务器返回了完整的响应（或连接彻底失败）后
    // 立即返回这个 QNetworkReply 对象的一个 reply 指针。
    // 此时Qt 网络管理器内部已经持有这个 reply 的指针了,finished信号会携带这一 reply 指针参数
    // void finished(QNetworkReply *reply);// Qt 内部声明（伪代码）
    mNetAccessManger->get(QNetworkRequest(url));

}

//通过 ipinfo.io 自动定位,如果定位失败，就默认使用“北京”的天气。
void ApiClient::getLocationByIP()
{
    //1. 创建一个临时的网络管理器（QNetworkAccessManager）
    auto *ipManager = new QNetworkAccessManager(this);
    //3. 在收到回复的槽函数（Lambda 表达式）里：
    connect(ipManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        //① 延时释放网络资源
        reply->deleteLater();
        ipManager->deleteLater();
        //②检查回复是否有错误（如网络不通、超时、HTTP 状态码错误等）。
        if (reply->error() != QNetworkReply::NoError) {
            getWeatherInfo("北京");
            return;
        }
        // ③ 解析 JSON，提取城市拼音
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QString cityPinyin = doc.object().value("city").toString();
        QString cityCode = WeatherTool::getCityCodeByPinyin(cityPinyin);//根据拼音查编码（用于 IP 定位）
        qDebug() << "自动定位到:" << cityPinyin << "code:" << cityCode;
        //如果 cityCode 为空（即拼音转换失败，根据该拼音查不到编码），同样默认使用“北京”，否则就用转换后的代码。
        getWeatherInfo(cityCode.isEmpty() ? "北京" : cityCode);
    });
    //2. 发送一个 GET 请求到 https://ipinfo.io/json
    ipManager->get(QNetworkRequest(QUrl("https://ipinfo.io/json")));
}

//HTTP 层：（是否能访问那个天气API网站）槽函数用于处理服务器返回的响应数据
void ApiClient::onReplied(QNetworkReply *reply)
{
    qDebug()<<"onReplied success";//表示槽函数 onReplied 被成功触发。(网络已到达)

    //当响应的状态码为200时,表示请求成功
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //reply->operation() 返回一个 QNetworkAccessManager::Operation 枚举值，表示本次网络请求的方式。
    qDebug()<<"operation:"<<reply->operation();     //请求方式,2表示Get请求;1表示Head请求;3表示Post请求
    qDebug()<<"status_code:"<<status_code;          //HTTP 状态码
    qDebug()<<"url:"<<reply->url();                 //url
    qDebug()<<"raw header:"<<reply->rawHeaderList();//header
    // Content-Encoding: gzip
    // 响应体被 gzip 压缩了。Qt 的 QNetworkAccessManager 默认会自动解压，之后 readAll() 拿到的直接是解压后的 JSON 文本，不用手动处理。
    // 但要注意，如果不支持解压，readAll() 得到的是压缩后的乱码。

    //判断请求是否出错（如网络不通、DNS 解析失败）或 HTTP 状态码不是 200 OK。
    if(reply->error() != QNetworkReply::NoError || status_code != 200)
    {
        qDebug()<<reply->errorString().toLatin1().data();
        if (m_retryCount == 0) {// 第一次失败才发射
            emit errorOccurred("请求数据失败，请检查网络连接~~！");
        }
        if (++m_retryCount <= MAX_RETRY_COUNT) {
            qDebug() << "后台重试" << m_retryCount << "/" << MAX_RETRY_COUNT;
            m_retryTimer->start(RETRY_INTERVAL_MS);// 后续静默重试，不再弹窗
        } else {
            qDebug() << "重试次数已耗尽";
            emit errorOccurred("重连网络失败！");
        }
    }
    else//没有出错
    {
        cancelRetry();//停止重试定时器，重置计数

        //获取响应信息并打印在控制台:
        //用 reply->readAll() 读取服务器返回的完整数据（这里就是天气的 JSON 字符串），并打印出来。
        QByteArray byteArray = reply->readAll();
        qDebug()<<"read all:\n"<<byteArray.data();
        parseJson(byteArray);
    }
    //最后必须调用 reply->deleteLater() 释放网络回复对象的内存，避免内存泄漏。
    reply->deleteLater();
}

// API 业务层：（是否能正确解析城市编码的天气）解析 JSON 并发射 weatherDataReady 信号
// ①当输入为呼伦贝尔市（http://t.weather.itboy.net/api/weather/city/101081000）时
// HTTP请求完全正常，但在响应体里面的"status"字段值为: 403
// 整个响应体为：
// {
//     "message": "CityId101081000不在返回之内,加QQ群：608222884，反馈问题。",
//     "status": 403
// }
// ②当输入一个API网站找不到的城市的编码(无效城市编码)时,如(http://t.weather.itboy.net/api/weather/city/10108101)
//  HTTP请求也是完全正常,但在响应体里面的"status"字段值为: 404
// 整个响应体为:
// {
//     "message": "Request resource not found.",
//     "status": 404
// }
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

    //检查这个http://t.weather.itboy.net/api/weather/city/城市编码 API的"status"这字段是否为200;

    // 返回200表示这个城市编码正确并且连接API成功;
    // 返回404本地有编码，但 API 未收录
    // 返回403表示API 知道这个城市，但不在服务范围内;
    if(rootObj.value("status").toInt() == 404){
        emit errorOccurred("请求数据失败，该城市在本地存在编码,但是API未收录该城市编码！");
        return;
    }
    else if(rootObj.value("status").toInt() == 403){//如呼伦贝尔市
        emit errorOccurred("请求数据失败，该城市不在返回之内,加QQ群：608222884，反馈问题。");
        return;
    }
    else if(rootObj.value("status").toInt() != 200){
        emit errorOccurred("请求数据失败，其他未知错误~请换个城市试试呢！");
        return;
    }

    //1. 解析城市
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    //2. 确定今天的基准日期（从 time 字段）
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonArray forecastArray = objData.value("forecast").toArray();

    QString timeStr = objData.value("time").toString();
    if (timeStr.isEmpty())
        timeStr = rootObj.value("time").toString();

    QDate todayDate;
    QDateTime dt = QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");
    if (dt.isValid())
        todayDate = dt.date();
    if (!todayDate.isValid()) {
        dt = QDateTime::fromString(timeStr, Qt::ISODate);
        if (dt.isValid())
            todayDate = dt.date();
    }
    if (!todayDate.isValid())
        todayDate = QDate::currentDate();

    //3. 在 forecast 中找到匹配今天的索引
    int todayIdx = -1;
    for (int i = 0; i < forecastArray.size(); i++) {
        QDate fd = QDate::fromString(
            forecastArray[i].toObject().value("ymd").toString(), "yyyy-MM-dd");
        if (fd == todayDate) { todayIdx = i; break; }
    }
    if (todayIdx < 0) todayIdx = 0;

    //4. 按对齐后的索引填充 mDay[0..6]
    auto parseDay = [](const QJsonObject &obj, Day &d) {
        d.date = obj.value("ymd").toString();
        d.week = obj.value("week").toString();
        d.type = obj.value("type").toString();
        d.sunrise = obj.value("sunrise").toString();
        d.sunset = obj.value("sunset").toString();
        d.notice = obj.value("notice").toString();
        QString s;
        QStringList parts = obj.value("high").toString().split(" ");
        s = parts.size() > 1 ? parts.at(1) : "";
        s = s.left(s.length()-1);
        d.high = s.toInt();
        parts = obj.value("low").toString().split(" ");
        s = parts.size() > 1 ? parts.at(1) : "";
        s = s.left(s.length()-1);
        d.low = s.toInt();
        d.fl = obj.value("fl").toString();
        d.fx = obj.value("fx").toString();
        d.aqi = obj.value("aqi").toDouble();
    };

    // 昨天
    if (todayIdx == 0)
        parseDay(objData.value("yesterday").toObject(), mDay[0]);
    else
        parseDay(forecastArray[todayIdx - 1].toObject(), mDay[0]);

    // 今天 + 未来六天
    int endIdx = qMin(todayIdx + 6, forecastArray.size());
    for (int i = todayIdx; i < endIdx; i++)
        parseDay(forecastArray[i].toObject(), mDay[i - todayIdx + 1]);

    //5. 解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    QJsonValue wenduVal = objData.value("wendu");
    mToday.wendu = wenduVal.toString().toDouble();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.pm10 = objData.value("pm10").toDouble();
    mToday.quality = objData.value("quality").toString();
    mToday.updateTime = objData.value("time").toString();
    if (mToday.updateTime.isEmpty())
        mToday.updateTime = rootObj.value("time").toString();
    if (mToday.updateTime.isEmpty())
        mToday.updateTime = rootObj.value("cityInfo").toObject().value("updateTime").toString();
    mToday.date = rootObj.value("date").toString();

    //6. 今天的数据来自对齐后的 mDay[1]
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //7. 更新UI
    emit weatherDataReady(mToday, mDay);//数据就绪后通知 Widget
}

void ApiClient::retry()
{
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + m_pendingCityCode);
    mNetAccessManger->get(QNetworkRequest(url));
}
