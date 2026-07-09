#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QString>
#include <QStringList>
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
    // 加载 json，解析为 mCityMap
    static void initCityMap()
    {
        //1.读取文件:从 city_weather.json 资源文件加载全国城市编码映射表
        QString filePath = ":/city_weather/city_weather.json";
        QFile file(filePath);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray city_weather_json = file.readAll();
        file.close();

        //2.解析并写入 map
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(city_weather_json,&err);
        if(err.error!=QJsonParseError::NoError) return;
        if(!doc.isArray()) return;
        QJsonArray cities = doc.array();

        QMap<QString,int> count;
        // 先统计重名次数，重名的 key 用 "朝阳(辽宁)" 格式；
        for(int i = 0; i < cities.size(); i++) {
            QString Name = cities[i].toObject().value("cityName").toString();
            count[Name]++;
        }

        for(int i = 0; i < cities.size(); i++) {
            QString Name = cities[i].toObject().value("cityName").toString();
            QString Province = cities[i].toObject().value("province").toString();
            QString Code = cities[i].toObject().value("cityCode").toString();
            if(Code.size() > 0) {
                if(count[Name] > 1) {
                    QString key = Name + "(" + Province + ")";
                    mCityMap.insert(key, Code);
                    m_cityAmbiguityMap[Name].append(key);
                } else {
                    mCityMap.insert(Name, Code);
                }
            }
        }
    }

    //根据中文名查编码，支持补/去 "市" 后缀，支持补/去 "县" 后缀
    static QString getCityCode(const QString &cityName){
        if(mCityMap.isEmpty()) initCityMap();

        auto tryFind = [&](const QString &key) -> QString {
            auto it = mCityMap.find(key);
            return it != mCityMap.end() ? it.value() : "";
        };

        QString code = tryFind(cityName);
        if(!code.isEmpty()) return code;

        code = tryFind(cityName + "市");
        if(!code.isEmpty()) return code;

        if(cityName.endsWith("市")) {
            code = tryFind(cityName.left(cityName.size() - 1));
            if(!code.isEmpty()) return code;
        }

        code = tryFind(cityName + "县");
        if(!code.isEmpty()) return code;

        if(cityName.endsWith("县")) {
            code = tryFind(cityName.left(cityName.size() - 1));
            if(!code.isEmpty()) return code;
        }

        if(!cityName.contains('(')) {
            QString base = cityName;
            if((base.endsWith("市") || base.endsWith("县")) && base.size() > 1)
                base = base.left(base.size() - 1);
            if(m_cityAmbiguityMap.contains(base))
                return "";
        }

        return "";
    }

    //记录歧义映射
    static QStringList getAmbiguousOptions(const QString &cityName) {
        if(mCityMap.isEmpty()) initCityMap();
        return m_cityAmbiguityMap.value(cityName);
    }

private:
    //提供静态方法根据城市中文名查询城市编码

    inline static QMap<QString,QString> mCityMap;
    inline static QMap<QString,QStringList> m_cityAmbiguityMap;
};

#endif // WEATHERTOOL_H
