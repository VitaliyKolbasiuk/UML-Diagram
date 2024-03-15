#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"

#include <QMouseEvent>

#define CIRCLE_RADIUS getGridWidth() / 13

Diagram::Diagram()
{
    setMinimumSize(getDiagramWidth() + 10, getDiagramHeight() + 10);

    setMouseTracking(true);
}

void Diagram::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::cyan);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    drawGrid(&painter);

    pen.setColor(Qt::yellow);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    gCurrentPainter = &painter;
    if (m_dragElement)
    {
        update();
        drawGridElement(*m_dragElement);
    }

    for (const auto& element : m_diagramElements)
    {
        if (&element != m_currentElement)
        {
            drawGridElement(element);
        }
    }

    if (m_currentElement)
    {
        drawCurrentElement(&painter);
    }
}

void Diagram::drawGrid(QPainter* painter)
{
    for (int x = 0; x <= getDiagramWidth(); x += getGridWidth())
    {
        painter->drawLine(x, 0, x, getDiagramHeight());
    }
    for (int y = 0; y <= getDiagramHeight(); y += getGridHeight())
    {
        painter->drawLine(0, y, getDiagramWidth(), y);
    }
}

void Diagram::drawGridElement(const DiagramElement& element)
{
    switch (element.type)
    {
        case ToolBoxModel::Element::Block:
            ToolBoxModel::Block{}.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;

        case ToolBoxModel::Element::If:
            ToolBoxModel::If{}.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;

        case ToolBoxModel::Element::For:
            ToolBoxModel::For{}.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;
    }
}

void Diagram::drawCurrentElement(QPainter* painter)
{
    QBrush brush(Qt::darkGray);
    painter->setBrush(brush);
    QPen pen(Qt::white);
    painter->setPen(pen);
    drawGridElement(*m_currentElement);

    int halfRadius = CIRCLE_RADIUS / 2;

    int leftPointX  = m_currentElement->column * getGridWidth() - halfRadius;
    int midPointX  = m_currentElement->column * getGridWidth() + getGridWidth() / 2 - halfRadius;
    int rightPointX = m_currentElement->column * getGridWidth() + getGridWidth() - halfRadius;

    int midPointY  = m_currentElement->row * getGridHeight() + getGridHeight() / 2 - halfRadius;
    int downPointY = m_currentElement->row * getGridHeight() + getGridHeight() - halfRadius;

    switch (m_currentElement->type)
    {
        case ToolBoxModel::Element::Block:
            painter->drawEllipse(midPointX, downPointY, CIRCLE_RADIUS, CIRCLE_RADIUS);
            break;

        case ToolBoxModel::Element::If:
            painter->drawEllipse(leftPointX,midPointY, CIRCLE_RADIUS, CIRCLE_RADIUS);

            painter->drawEllipse(midPointX,downPointY, CIRCLE_RADIUS, CIRCLE_RADIUS);

            painter->drawEllipse(rightPointX, midPointY, CIRCLE_RADIUS, CIRCLE_RADIUS);
            break;

        case ToolBoxModel::Element::For:
            painter->drawEllipse(midPointX,downPointY,CIRCLE_RADIUS, CIRCLE_RADIUS);

            painter->drawEllipse(rightPointX,midPointY,CIRCLE_RADIUS, CIRCLE_RADIUS);
            break;
    }
}

void Diagram::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "Diagram mouse pressed";

    m_currentElement = isElementClicked(event->pos());
}

DiagramElement* Diagram::isElementClicked(QPoint mousePos)
{
    int i = mousePos.x() / getGridWidth();
    int j = mousePos.y() / getGridHeight();

    for(auto& element : m_diagramElements)
    {
        if (element.column == i && element.row == j)
        {
            update();
            return &element;
        }
    }
    return nullptr;
}

void Diagram::mouseMoveEvent(QMouseEvent* event)
{
    //qDebug() << "Diagram mouse move";
}

void Diagram::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << "Diagram mouse release";
}

void Diagram::setCurrentElement(ToolBoxModel::Element::Type type)
{
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    if (! isWithinDiagramArea(mousePos))
    {
        return;
    }

    int i = mousePos.x() / getGridWidth();
    int j = mousePos.y()  / getGridHeight();
    if (!m_dragElement)
    {
        m_dragElement = new DiagramElement{type, i, j};
    }
    else
    {
        m_dragElement->column = i;
        m_dragElement->row = j;
    }
}

void Diagram::currentItemReleased()
{
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    if (!isWithinDiagramArea(mousePos))
    {
        deleteDragElement();
        return;
    }

    int i = mousePos.x() / getGridWidth();
    int j = mousePos.y()  / getGridHeight();
    for(const auto& element : m_diagramElements)
    {
        if (element.column == i && element.row == j)
        {
            deleteDragElement();
            return;
        }
    }

    m_dragElement->column = i;
    m_dragElement->row = j;
    m_diagramElements.emplace_back(*m_dragElement);
    deleteDragElement();
}

bool Diagram::isWithinDiagramArea(QPoint point) const
{
    return point.x() >= 0 && point.x() < getDiagramWidth() &&
           point.y() >= 0 && point.y() < getDiagramHeight();
}

void Diagram::deleteDragElement()
{
    delete m_dragElement;
    m_dragElement = nullptr;
}
