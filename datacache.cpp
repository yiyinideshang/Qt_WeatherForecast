#include "datacache.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

DataCache::DataCache(QObject *parent)
    : QObject(parent)
{
    initDb();//构造函数，调用 initDb 建表
}

DataCache::~DataCache()
{
    //析构时关闭数据库
    if (m_db.isOpen())
        m_db.close();
}

//创建 cache_today 和 cache_day 两张表
void DataCache::initDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "weather_cache");
    m_db.setDatabaseName("weather_cache.db");

    //打开数据库
    if (!m_db.open()) {
        qDebug() << "缓存数据库打开失败:" << m_db.lastError().text();
        return;
    }

    QSqlQuery query(m_db);
    query.exec(
        "CREATE TABLE IF NOT EXISTS cache_today ("
        "city_code TEXT PRIMARY KEY,"
        "date TEXT, city TEXT, ganmao TEXT,"
        "wendu REAL, shidu TEXT, pm25 INTEGER, quality TEXT,"
        "type TEXT, fl TEXT, fx TEXT, high INTEGER, low INTEGER,"
        "update_time TEXT, pm10 INTEGER DEFAULT 0, last_access_time TEXT)"
    );
    query.exec(
        "CREATE TABLE IF NOT EXISTS cache_day ("
        "city_code TEXT,"
        "idx INTEGER,"
        "date TEXT, week TEXT, type TEXT,"
        "high INTEGER, low INTEGER, fl TEXT, fx TEXT, aqi INTEGER,"
        "sunrise TEXT DEFAULT '', sunset TEXT DEFAULT '', notice TEXT DEFAULT '',"
        "PRIMARY KEY (city_code, idx))"
    );
}

// ── 两个字段的区别 ──────────────────────────────────────────────
// update_time      = API 数据更新时间（天气服务端最后刷新数据的时间）
//                     来源：today.updateTime（API JSON 的 time 字段）
//                     用途：仅用于 UI 展示（lblUpdateTime），告诉用户数据新鲜度
//
// last_access_time = 缓存最后访问时间（本地写入或读取缓存的时刻）
//                     来源：QDateTime::currentDateTime()
//                     用途：① 过期判断（2 小时 TTL）  ② LRU 淘汰排序
// ────────────────────────────────────────────────────────────────

//检查缓存是否过期的工具函数。
//参数 timeStr：传入的是 last_access_time（缓存访问时间），用于判断距上次访问是否超过 2 小时
static bool isExpired(const QString &timeStr)
{
    QDateTime dt = QDateTime::fromString(timeStr, Qt::ISODate);//先用 ISO 格式解析时间字符串
    if (!dt.isValid())
        dt = QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");
    return !dt.isValid() || dt.secsTo(QDateTime::currentDateTime()) > 7200;//解析失败就换另一种格式重试
    // 返回 true（已过期）的条件：时间格式无法解析，或者当前时间减去缓存时间已经超过 7200 秒（2 小时）
}

//读缓存，检查过期，返回是否命中
bool DataCache::load(const QString &cityCode, Today &today, Day day[7])
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM cache_today WHERE city_code = ?");
    query.addBindValue(cityCode);
    if (!query.exec() || !query.next())
        return false;

    if (isExpired(query.value("last_access_time").toString()))
        return false; // 缓存过期，视为未命中

    today.date = query.value("date").toString();
    today.city = query.value("city").toString();
    today.ganmao = query.value("ganmao").toString();
    today.wendu = query.value("wendu").toDouble();
    today.shidu = query.value("shidu").toString();
    today.pm25 = query.value("pm25").toInt();
    today.pm10 = query.value("pm10").toInt();
    today.quality = query.value("quality").toString();
    today.type = query.value("type").toString();
    today.fl = query.value("fl").toString();
    today.fx = query.value("fx").toString();
    today.high = query.value("high").toInt();
    today.low = query.value("low").toInt();
    today.updateTime = query.value("update_time").toString();

    query.prepare("SELECT * FROM cache_day WHERE city_code = ? ORDER BY idx");
    query.addBindValue(cityCode);
    if (!query.exec())
        return false;

    int i = 0;
    while (query.next() && i < 7) {
        day[i].date = query.value("date").toString();
        day[i].week = query.value("week").toString();
        day[i].type = query.value("type").toString();
        day[i].high = query.value("high").toInt();
        day[i].low = query.value("low").toInt();
        day[i].fl = query.value("fl").toString();
        day[i].fx = query.value("fx").toString();
        day[i].aqi = query.value("aqi").toInt();
        day[i].sunrise = query.value("sunrise").toString();
        day[i].sunset = query.value("sunset").toString();
        day[i].notice = query.value("notice").toString();
        ++i;
    }

    // 缓存命中，刷新 last_access_time 为当前时间，实现真正的 LRU（最近访问而非最近写入）
    query.prepare("UPDATE cache_today SET last_access_time = ? WHERE city_code = ?");
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(cityCode);
    query.exec();

    qDebug() << "缓存命中:" << cityCode << today.city;
    return i == 7;
}

//写缓存（INSERT OR REPLACE），完成后检查并淘汰
void DataCache::save(const QString &cityCode, const Today &today, const Day day[7])
{
    if (!m_db.isOpen())
        return;

    QSqlQuery query(m_db);

    query.prepare(
        "INSERT OR REPLACE INTO cache_today "
        "(city_code, date, city, ganmao, wendu, shidu, pm25, quality, "
        "type, fl, fx, high, low, update_time, pm10, last_access_time) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );
    query.addBindValue(cityCode);
    query.addBindValue(today.date);
    query.addBindValue(today.city);
    query.addBindValue(today.ganmao);
    query.addBindValue(today.wendu);
    query.addBindValue(today.shidu);
    query.addBindValue(today.pm25);
    query.addBindValue(today.quality);
    query.addBindValue(today.type);
    query.addBindValue(today.fl);
    query.addBindValue(today.fx);
    query.addBindValue(today.high);
    query.addBindValue(today.low);
    query.addBindValue(today.updateTime.isEmpty()
        ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
        : today.updateTime);
    query.addBindValue(today.pm10);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.exec();

    query.prepare("DELETE FROM cache_day WHERE city_code = ?");
    query.addBindValue(cityCode);
    query.exec();

    for (int i = 0; i < 7; ++i) {
        query.prepare(
            "INSERT OR REPLACE INTO cache_day "
            "(city_code, idx, date, week, type, high, low, fl, fx, aqi, "
            "sunrise, sunset, notice) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)"
        );
        query.addBindValue(cityCode);
        query.addBindValue(i);
        query.addBindValue(day[i].date);
        query.addBindValue(day[i].week);
        query.addBindValue(day[i].type);
        query.addBindValue(day[i].high);
        query.addBindValue(day[i].low);
        query.addBindValue(day[i].fl);
        query.addBindValue(day[i].fx);
        query.addBindValue(day[i].aqi);
        query.addBindValue(day[i].sunrise);
        query.addBindValue(day[i].sunset);
        query.addBindValue(day[i].notice);
        query.exec();
    }

    evictIfNeeded();

    qDebug() << "缓存已保存:" << cityCode << today.city << "\n";

}

//返回所有缓存城市的 (cityCode, cityName) 列表，按 last_access_time 降序
QList<QPair<QString,QString>> DataCache::getAllCachedCities()
{
    QList<QPair<QString,QString>> result;
    if (!m_db.isOpen())
        return result;

    QSqlQuery query(m_db);
    //DESC 降序，即从大到小、从晚到早。对应的 ASC（ascending）是升序，默认排序方式就是 ASC。
    query.exec("SELECT city_code, city FROM cache_today ORDER BY last_access_time DESC");
    while (query.next())
        result.append({query.value(0).toString(), query.value(1).toString()});
    return result;
}

//清空 cache_today 和 cache_day 所有记录
void DataCache::clearAll()
{
    // 检查数据库是否已打开（m_db 是 QSqlDatabase 对象）
    if (!m_db.isOpen())
        return;//未打开直接返回

    QSqlQuery query(m_db);//创建一个绑定到当前数据库连接的查询对象。
    //执行数据库删除操作
    query.exec("DELETE FROM cache_today");// 删除当天天气表的所有数据
    query.exec("DELETE FROM cache_day");// 删除多天预报表的所有数据
    qDebug() << "所有缓存已清除";
}

//缓存数量 > MAX_ENTRIES 时淘汰最早一条记录（LRU）
void DataCache::evictIfNeeded()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM cache_today");
    if (query.next() && query.value(0).toInt() <= MAX_ENTRIES)
        return;

    query.exec("SELECT city_code FROM cache_today ORDER BY last_access_time ASC LIMIT 1");
    if (query.next()) {
        QString oldest = query.value(0).toString();
        QSqlQuery del(m_db);
        del.prepare("DELETE FROM cache_today WHERE city_code = ?");
        del.addBindValue(oldest);
        del.exec();
        del.prepare("DELETE FROM cache_day WHERE city_code = ?");
        del.addBindValue(oldest);
        del.exec();
        qDebug() << "淘汰缓存:" << oldest;
    }
}
