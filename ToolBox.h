#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

class DragObject;

class ToolBox : public QWidget
{
    DragObject* m_dragObject;
public:
    ToolBox();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void setDragObjectRef(DragObject& dragObject);
};
