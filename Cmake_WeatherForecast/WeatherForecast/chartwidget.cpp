#include "chartwidget.h"
#include <QPainter>

#define INCREMENT 3
#define POINT_RADIUS 3
#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 12

ChartWidget::ChartWidget(const QColor &color, QWidget *parent)
    : QWidget(parent), m_color(color)
{
}

void ChartWidget::setData(const int temps[6])
{
    for (int i = 0; i < 6; ++i) {
        m_temps[i] = temps[i];
    }
    update();
}

void ChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int pointX[6];
    int stepX = width() / 7;
    for (int i = 0; i < 6; ++i) {
        pointX[i] = stepX * (i + 1);
    }

    int tempSum = 0;
    for (int i = 0; i < 6; ++i) {
        tempSum += m_temps[i];
    }
    int tempAverage = tempSum / 6;

    int pointY[6];
    int yCenter = height() / 2;
    for (int i = 0; i < 6; ++i) {
        pointY[i] = yCenter - ((m_temps[i] - tempAverage) * INCREMENT);
    }

    painter.save();
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(m_color);
    painter.setPen(pen);
    painter.setBrush(m_color);

    for (int i = 0; i < 6; ++i) {
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);
        painter.drawText(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y,
                         QString::number(m_temps[i]) + "°");
    }

    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        } else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i + 1], pointY[i + 1]);
    }
    painter.restore();
}
