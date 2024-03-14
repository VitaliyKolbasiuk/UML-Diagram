#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#include "ToolBoxModel.h"

inline int getDiagramWidth()
{
    return 500;
}

inline int getDiagramHeight()
{
    return 600;
}

inline int getGridWidth()
{
    return 100;
}

inline int getGridHeight()
{
    return 50;
}

struct DiagramElement
{
    ToolBoxModel::Element::Type type;
    int column;
    int row;
};

class Diagram : public QWidget
{
    std::vector<DiagramElement> m_diagramElements;
    DiagramElement* m_dragElement = nullptr;
    DiagramElement* m_currentElement = nullptr;
//    std::vector<>   m_arrow;

public:
    Diagram();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void setCurrentElement(ToolBoxModel::Element::Type type);
    void currentItemReleased();

private:
    void drawGridElement(const DiagramElement& element);
    void drawGrid(QPainter* painter);
    DiagramElement* isElementClicked(QPoint mousePos);
    void drawCurrentElement();
};
