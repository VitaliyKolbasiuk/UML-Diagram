#pragma once

#include "ToolBoxModel.h"

#include <QWidget>
#include <map>

using Connector = std::vector<QPoint>;

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

    bool onInputCircleCollision(const QPoint mousePos, Connector& connector);
};

struct Graph
{
    struct Node
    {
        DiagramElement* m_element;
    };

    std::map<std::pair<Node, Node>> m_graph;
};

class Diagram : public QWidget
{
    std::vector<DiagramElement> m_diagramElements;
    std::vector<Connector>      m_connectors;
    Graph                       m_graph;

    DiagramElement*             m_dragElement = nullptr;
    DiagramElement*             m_currentElement = nullptr;
    DiagramElement*             m_inputElement = nullptr;

    Connector                   m_currentArrow;
    QPoint                      m_newArrowPoint;

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
    DiagramElement* onElementClicked(QPoint mousePos);
    void drawCurrentElement(QPainter* painter);
    bool isWithinDiagramArea(const QPoint point) const;
    void deleteDragElement();
    std::vector<QPoint> createCircles();
    void drawCurrElementCircles(QPainter* painter, const std::vector<QPoint>& points);
    QPoint onCircleCollision(const QPoint mousePos);
    void drawInputElement(QPainter* painter);
    bool onInputCircleCollision(const QPoint mousePos, Connector& connector);
};
