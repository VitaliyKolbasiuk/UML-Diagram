#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"

#include <QMouseEvent>

#define CIRCLE_RADIUS getGridWidth() / 15

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

    drawCurrElementCircles(painter, createCircles());
}

void Diagram::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "Diagram mouse pressed";

    if (m_currentElement && onCircleCollision(event->pos()))
    {
        // TODO : START TO DRAW ARROWS
    }
    else
    {
        m_currentElement = onElementClicked(event->pos());
        update();
    }
}

DiagramElement* Diagram::onElementClicked(QPoint mousePos)
{
    int i = mousePos.x() / getGridWidth();
    int j = mousePos.y() / getGridHeight();

    for(auto& element : m_diagramElements)
    {
        if (element.column == i && element.row == j)
        {
            return &element;
        }
    }
    return nullptr;
}

bool Diagram::onCircleCollision(const QPoint mousePos)
{
    std::vector<QPoint> points = createCircles();

    for (const auto& point : points)
    {

        if ((mousePos.x() - point.x()) * (mousePos.x() - point.x()) +
            (mousePos.y() - point.y()) * (mousePos.y() - point.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
        {
            return true;
        }
    }
    return false;
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

bool Diagram::isWithinDiagramArea(const QPoint point) const
{
    return point.x() >= 0 && point.x() < getDiagramWidth() &&
           point.y() >= 0 && point.y() < getDiagramHeight();
}

void Diagram::deleteDragElement()
{
    delete m_dragElement;
    m_dragElement = nullptr;
}

std::vector<QPoint> Diagram::createCircles()
{
    int leftPointX  = m_currentElement->column * getGridWidth();
    int midPointX  = m_currentElement->column * getGridWidth() + getGridWidth() / 2;
    int rightPointX = m_currentElement->column * getGridWidth() + getGridWidth();

    int midPointY  = m_currentElement->row * getGridHeight() + getGridHeight() / 2 ;
    int downPointY = m_currentElement->row * getGridHeight() + getGridHeight();
    switch (m_currentElement->type)
    {
        case ToolBoxModel::Element::Block:
            return {QPoint(leftPointX, midPointY)};
        case ToolBoxModel::Element::If:
            return {QPoint(leftPointX, midPointY),
                    QPoint(midPointX, downPointY),
                    QPoint(rightPointX, midPointY)};
        case ToolBoxModel::Element::For:
            return {QPoint(midPointX, downPointY),
                    QPoint(rightPointX, midPointY)};
        default:
            return {};
    }
}

void Diagram::drawCurrElementCircles(QPainter* painter, const std::vector<QPoint> &points)
{
    for (const auto& point : points)
    {
        painter->drawEllipse(point, CIRCLE_RADIUS, CIRCLE_RADIUS);
    }
}