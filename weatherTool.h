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
    // 加载 json，解析为 mCityMap 和 mPinyinMap

    static void initCityMap()
    {
        //1.读取文件:从 city_weather.json 资源文件加载全国城市编码映射表
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
            QString Pinyin = cities[i].toObject().value("cityPinyin").toString();
            if(Code.size()>0){
                mCityMap.insert(Name,Code);
                mPinyinMap.insert(Pinyin,Code);
            }
        }
    }

//根据中文名查编码，支持补/去 "市" 后缀，支持补/去 "县" 后缀
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
        if(it == mCityMap.end()){
            it = mCityMap.find(cityName+"县");
        }
        if(it == mCityMap.end() && cityName.endsWith("县")){
            it = mCityMap.find(cityName.left(cityName.size() - 1));
        }
        if(it!=mCityMap.end()){
            return it.value();
        }
        return "";
    }

//根据拼音查编码（用于 IP 定位）
    static QString getCityCodeByPinyin(const QString &pinyin){
        if(mCityMap.isEmpty()){
            initCityMap();
        }
        return mPinyinMap.value(pinyin.toLower());
    }
private:
    //提供静态方法根据城市中文名或拼音查询城市编码
    inline static QMap<QString,QString> mCityMap;//城市中文名到编码
    inline static QMap<QString,QString> mPinyinMap;//城市拼音到编码
};


// QMap<QString,QString> WeatherTool::mCityMap = {};
#endif // WEATHERTOOL_H
