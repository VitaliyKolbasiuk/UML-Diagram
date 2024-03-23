#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"

#include <QMouseEvent>

#define CIRCLE_RADIUS getGridWidth() / 20

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

    if (!m_currentArrow.empty())
    {
        QPainterPath path;

        path.moveTo(m_currentArrow.front());
        for (int i = 1; i < m_currentArrow.size(); ++i)
        {
            path.lineTo(m_currentArrow[i]);
        }
        path.lineTo(m_newArrowPoint);
        painter.drawPath(path);
        update();
    }

    for (const auto& arrow : m_connectors)
    {
        assert(!arrow.empty());

        QPainterPath path;

        for (int i = 1; i < arrow.size(); ++i)
        {
            path.moveTo(arrow[i - 1]);
            path.lineTo(arrow[i]);
        }
        painter.drawPath(path);
    }

    if (m_inputElement)
    {
        drawInputElement(&painter);
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
        case ToolBoxModel::Element::Function:
            ToolBoxModel::Function{}.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;

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
    if (ConnectionPoint connectionPoint = onCircleCollision(event->pos()); !point.isNull())
    {
        m_currentArrow.emplace_back(point);
        m_newArrowPoint = point;
    }
    else if (onInputCircleCollision(event->pos(), m_currentArrow))
    {
        m_connectors.emplace_back(m_currentArrow);

        m_currentArrow.clear();
        m_inputElement = nullptr;
        m_currentElement = nullptr;
        m_newArrowPoint = QPoint();
        update();
    }
    else if (!m_currentArrow.empty())
    {
        int x = event->pos().x() / getGridWidth() * getGridWidth() + getGridWidth() / 2;
        int y = event->pos().y() / getGridHeight() * getGridHeight() + getGridHeight() / 2;
        m_currentArrow.emplace_back(ConnectionPoint{QPoint(x, y)});
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

ConnectionPoint Diagram::onCircleCollision(const QPoint mousePos)
{
    if (m_currentElement)
    {
        std::vector<ConnectionPoint> points = createCircles();

        for (const auto &point: points)
        {
            // CIRCLE COLLISION
            if ((mousePos.x() - point.m_point.x()) * (mousePos.x() - point.m_point.x()) +
                (mousePos.y() - point.m_point.y()) * (mousePos.y() - point.m_point.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                return point;
            }
        }
    }
    return QPoint();
}

void Diagram::mouseMoveEvent(QMouseEvent* event)
{
    DiagramElement* elementPtr = onElementClicked(event->pos());
    ConnectionPoint connectionPoint = onCircleCollision(event->pos());

    if(!m_currentArrow.empty())
    {
        if (elementPtr && elementPtr != m_currentElement)
        {
            m_inputElement = elementPtr;
            update();
        }
        else
        {
            m_inputElement = nullptr;
        }
        m_newArrowPoint = event->pos();
    }

    if (!elementPtr && point.m_point.isNull() && !m_inputElement)
    {
        m_currentElement = nullptr;
        update();
    }
    else if(!m_currentElement && elementPtr && !m_inputElement)
    {
        m_currentElement = elementPtr;
        update();
    }
}

void Diagram::mouseReleaseEvent(QMouseEvent* event)
{
    //qDebug() << "Diagram mouse release";
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

std::vector<ConnectionPoint> Diagram::createCircles()
{
    int leftPointX  = m_currentElement->column * getGridWidth();
    int midPointX  = m_currentElement->column * getGridWidth() + getGridWidth() / 2;
    int rightPointX = m_currentElement->column * getGridWidth() + getGridWidth();

    int midPointY  = m_currentElement->row * getGridHeight() + getGridHeight() / 2 ;
    int downPointY = m_currentElement->row * getGridHeight() + getGridHeight();
    switch (m_currentElement->type)
    {
        case ToolBoxModel::Element::Function:
            return {{QPoint(midPointX, downPointY), m_currentElement, ConnectionPoint::output}};
        case ToolBoxModel::Element::Block:
            return {{QPoint(leftPointX, midPointY), m_currentElement, ConnectionPoint::output}};

        case ToolBoxModel::Element::If:
            return {{QPoint(leftPointX, midPointY), m_currentElement, ConnectionPoint::no},
                    {QPoint(midPointX, downPointY),  m_currentElement, ConnectionPoint::yes},
                    {QPoint(rightPointX, midPointY),  m_currentElement, ConnectionPoint::no}};

        case ToolBoxModel::Element::For:
            return {{QPoint(midPointX, downPointY), m_currentElement, ConnectionPoint::for_body},
                    {QPoint(rightPointX, midPointY),  m_currentElement, ConnectionPoint::output}};
    }
}

void Diagram::drawCurrElementCircles(QPainter* painter, const std::vector<ConnectionPoint>& points)
{
    for (const auto& point : points)
    {
        painter->drawEllipse(point.m_point, CIRCLE_RADIUS, CIRCLE_RADIUS);
    }
}

void Diagram::drawInputElement(QPainter* painter)
{
    QBrush brush(Qt::darkGray);
    painter->setBrush(brush);
    QPen pen(Qt::white);
    painter->setPen(pen);

    drawGridElement(*m_inputElement);

    if (m_inputElement->type != ToolBoxModel::Element::Function)
    {
        int x = m_inputElement->column * getGridWidth() + getGridWidth() / 2;
        int y = m_inputElement->row * getGridHeight();
        drawCurrElementCircles(painter, {{QPoint(x, y), nullptr, ConnectionPoint::no}});
    }
}

bool Diagram::onInputCircleCollision(const QPoint mousePos, Connector& currentArrow)
{
    if (m_inputElement)
    {
        m_inputElement->onInputCircleCollision(mousePos, currentArrow);
        return true;
    }
    return false;
}

bool DiagramElement::onInputCircleCollision(const QPoint mousePos, Connector& currentArrow)
{
    switch(type)
    {
        case ToolBoxModel::Element::Function:
            break;
        case ToolBoxModel::Element::Block:
        case ToolBoxModel::Element::If:
        {
            int x = column * getGridWidth() + getGridWidth() / 2;
            int y = row * getGridHeight();
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                    (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.emplace_back(QPoint(x, y));
                return true;
            }
            break;
        }
        case ToolBoxModel::Element::For:
            int x = column * getGridWidth() + getGridWidth() / 2;
            int y = row * getGridHeight();
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                    (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.emplace_back(QPoint(x, y));
                return true;
            }

            x = column * getGridWidth();
            y = row * getGridHeight() + getGridHeight() / 2;
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                    (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.emplace_back(QPoint(x, y));
                return true;
            }
            break;
    }
    return false;
}


