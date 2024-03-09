#include "Diagram.h"

#include <QPainterPath>

Diagram::Diagram()
{
    setMinimumSize(getDiagramWidth() + 10, getDiagramHeight() + 10);
}

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

    painter->drawPath(path);
    for (int x = 0; x <= getDiagramWidth(); x += getGridWidth()) {
        path.moveTo(x, 0);
        path.lineTo(x, getDiagramHeight());
    }
    for (int y = 0; y <= getDiagramHeight(); y += getGridHeight()) {
        path.moveTo(0, y);
        path.lineTo(getDiagramWidth(), y);
    }
    painter->drawPath(path);

    //resize(getDiagramWidth(), getDiagramHeight());
}

void Diagram::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Diagram mouse pressed";
}

void Diagram::mouseMoveEvent(QMouseEvent *event)
{
    setMouseTracking(true);
    qDebug() << "Diagram mouse move";
}

void Diagram::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Diagram mouse release";
}
