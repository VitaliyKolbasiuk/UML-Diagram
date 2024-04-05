#pragma once

#include "ToolBoxModel.h"

#include <QWidget>
#include <map>

struct ConnectionPoint;
using Connector = std::vector<ConnectionPoint>;

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

    std::map<Node, Node> m_graph;
};

struct ConnectionPoint
{
    QPoint m_point;
    DiagramElement* m_element = nullptr;
    enum {input, yes, no, for_body, output} m_type = output;
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
    Graph& generateGraph();
    const DiagramElement* findFirstElement() const;

private:
    void drawGridElement(const DiagramElement& element);
    void drawGrid(QPainter* painter);
    DiagramElement* onElementClicked(QPoint mousePos);
    void drawCurrentElement(QPainter* painter);
    bool isWithinDiagramArea(const QPoint point) const;
    void deleteDragElement();
    std::vector<ConnectionPoint> createCircles() const;
    void drawCurrElementCircles(QPainter* painter, const std::vector<ConnectionPoint>& points);
    ConnectionPoint onCircleCollision(const QPoint mousePos);
    void drawInputElement(QPainter* painter);
    bool onInputCircleCollision(const QPoint mousePos, Connector& connector);


public:
    std::vector<const Connector*> getInputConnector(const DiagramElement* element) const;
    const Connector* getOutputConnector(const DiagramElement* element) const;
    const Connector* getYesOutputConnector(const DiagramElement* element) const;
    const Connector* getNoOutputConnector(const DiagramElement* element) const;
    const Connector* getBodyOutputConnector(const DiagramElement* element) const;
};
