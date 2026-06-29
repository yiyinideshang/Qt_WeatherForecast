#ifndef WEATHERDATA_H
#define WEATHERDATA_H
#include <QString>

class Today{
public:
    Today(){
        date = "2022-10-20";
        city = "广州";

        ganmao="感冒指数";

        wendu = 0;
        shidu = "0%";
        pm25=0;
        quality="无数据";

        type = "多云";

        fl = "2级";
        fx = "南风";

        high = 30;
        low = 18;
    }
    QString date;
    QString city;

    QString ganmao;

    double wendu;
    QString shidu;
    int pm25;
    QString quality;

    QString type;

    QString fl;
    QString fx;

    int high;
    int low;
};
class Day{
public:
    Day(){
        date = "2022-10-20";
        week = "周五";

        type = "多云";

        high = 0;
        low = 0;
        fl = "2级";
        fx = "南风";

        aqi = 0;//天气指数:良,轻度,重度...
    }
    QString date;
    QString week;

    QString type;

    QString fl;
    QString fx;

    int high;
    int low;
    int aqi;
};


#endif // WEATHERDATA_H
