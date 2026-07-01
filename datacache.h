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
    explicit DataCache(QObject *parent = nullptr);
    ~DataCache();

    bool load(const QString &cityCode, Today &today, Day day[6]);
    void save(const QString &cityCode, const Today &today, const Day day[6]);
    QList<QPair<QString,QString>> getAllCachedCities();
    void clearAll();

private:
    void initDb();
    void evictIfNeeded();

    QSqlDatabase m_db;
    static constexpr int MAX_ENTRIES = 5;
    static constexpr int CACHE_HOURS = 2;
};

#endif
