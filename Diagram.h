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

struct GridElement
{
    ToolBoxModel::Element::Type type;
    int xOffset;
    int yOffset;
};

class Diagram : public QWidget
{
    std::vector<GridElement> m_gridElements;
    GridElement* m_currGridElement = nullptr;

public:
    Diagram();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void setCurrentElement(ToolBoxModel::Element::Type type, int xOffset, int yOffset);
    void currentItemReleased(int xOffset, int yOffset);
private:
    void drawGridElement(QPainter* painter, const GridElement& element);
    void drawGrid(QPainter* painter);
};
