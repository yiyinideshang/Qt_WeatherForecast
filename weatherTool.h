#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QString>
#include <QMap>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QStringDecoder>

class WeatherTool{
public:
    static void initCityMap()
    {
        //1.读取文件
        QString filePath = ":/city_weather/city_weather.json";
        QFile file(filePath);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray city_weather_json = file.readAll();
        file.close();

        // 从GBK转UTF-8
        // QStringDecoder decoder(QStringConverter::System);           // 中文Windows = GBK
        // QString text = decoder(city_weather_json);                  // 解码
        // QByteArray utf8Data = text.toUtf8();                        // 转UTF-8

        //2.解析并写入map
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(city_weather_json,&err);
        if(err.error!=QJsonParseError::NoError){
            return;
        }
        if(!doc.isArray()){
            return;
        }
        QJsonArray cities = doc.array();
        for(int i = 0;i<cities.size();i++){
            // QJsonObject
            QString Name = cities[i].toObject().value("cityName").toString();
            QString Code = cities[i].toObject().value("cityCode").toString();
            if(Code.size()>0){
                mCityMap.insert(Name,Code);
            }
        }
    }

    static QString getCityCode(QString cityName){
        if(mCityMap.isEmpty()){
            initCityMap();
        }
        QMap<QString,QString>::iterator it = mCityMap.find(cityName);
        if(it == mCityMap.end()){
            it = mCityMap.find(cityName+"市");
        }
        if(it == mCityMap.end() && cityName.endsWith("市")){
            it = mCityMap.find(cityName.left(cityName.size() - 1));
        }
        if(it!=mCityMap.end()){
            return it.value();
        }
        return "";
    }
private:
    static QMap<QString,QString> mCityMap;
};


QMap<QString,QString> WeatherTool::mCityMap = {};
#endif // WEATHERTOOL_H
