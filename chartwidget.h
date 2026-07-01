#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QColor>

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(const QColor &color, QWidget *parent = nullptr);
    void setData(const int temps[6]);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_temps[6] = {};
    QColor m_color;
};

#endif
