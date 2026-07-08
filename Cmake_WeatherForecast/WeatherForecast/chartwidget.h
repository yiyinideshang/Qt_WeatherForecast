#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QColor>

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(const QColor &color, QWidget *parent = nullptr);//构造函数，设置曲线颜色
    void setData(const int temps[7]);//设置温度数据，触发重绘

protected:
    //重写父类的paintEvent 重绘事件
    void paintEvent(QPaintEvent *event) override;//核心绘制：计算坐标、画圆点、画折线

private:
    int m_temps[7] = {};//6 天的温度值
    QColor m_color;//曲线颜色（橙色/青色）
};

#endif
