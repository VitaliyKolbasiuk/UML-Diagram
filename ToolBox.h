#pragma once

#include <QWidget>

class DragObject;
class Diagram;

class ToolBox : public QWidget
{
    DragObject* m_dragObject;
    Diagram*    m_diagram;
    
public:
    ToolBox();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void setDragObjectRef(DragObject& dragObject);
    void setDiagramRef(QWidget& diagram);

    const Diagram* getDiagram()
    {
        return m_diagram;
    }
};
