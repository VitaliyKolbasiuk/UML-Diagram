#include <QPainterPath>
#include "Diagram.h"

void Diagram::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::cyan);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    drawGrid(&painter);
}

void Diagram::drawGrid(QPainter* painter)
{
    QPainterPath path;
    path.moveTo(200, 0);
    path.lineTo(200, 500);

    painter->drawPath(path);
//    for (int i = 0, x = 0, y = 0; i < 50 * 100; ++i, ++x, ++y)
//    {
//        if (x > 100) x = 0;
//        if (y > 50)  y = 0;
//        path.
//    }
}
