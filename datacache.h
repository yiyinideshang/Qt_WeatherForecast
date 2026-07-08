#ifndef DATACACHE_H
#define DATACACHE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include <QPair>
#include "weatherdata.h"

class DataCache : public QObject
{
    Q_OBJECT
public:
    explicit DataCache(QObject *parent = nullptr);//构造函数，调用 initDb 建表
    ~DataCache();//析构时关闭数据库

    bool load(const QString &cityCode, Today &today, Day day[7]);//读缓存，检查过期，返回是否命中
    void save(const QString &cityCode, const Today &today, const Day day[7]);//写缓存（INSERT OR REPLACE），完成后检查并淘汰
    //返回所有缓存城市的 (cityCode, cityName) 列表，按 update_time 降序
    QList<QPair<QString,QString>> getAllCachedCities();

    //清空 cache_today 和 cache_day 所有记录
    void clearAll();

private:
    void initDb();//创建 cache_today 和 cache_day 两张表
    void evictIfNeeded();//缓存数量 > MAX_ENTRIES 时淘汰最早一条记录（LRU）

    QSqlDatabase m_db;//SQLite 数据库连接
    static constexpr int MAX_ENTRIES = 5;//最大缓存城市数（5）
    static constexpr int CACHE_HOURS = 2;//缓存有效期（2 小时）
};

#endif
