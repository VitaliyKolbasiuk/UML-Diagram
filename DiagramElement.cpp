#include "DiagramElement.h"
#include "ConnectionPoint.h"

#include <QPainterPath>

bool DiagramElement::onInputCircleCollision(const QPoint& mousePos, Connector& currentArrow) const
{
    switch(m_type)
    {
        case ToolBoxModel::Element::Function:
            break;
        case ToolBoxModel::Element::Block:
        case ToolBoxModel::Element::If:
        {
            int x = m_column * getGridWidth() + getGridWidth() / 2;
            int y = m_row * getGridHeight();
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.pop_back();
                currentArrow.emplace_back(ConnectionPoint{QPoint(x, y), nullptr, ConnectionPoint::no});
                return true;
            }
            break;
        }
        case ToolBoxModel::Element::For:
            int x = m_column * getGridWidth() + getGridWidth() / 2;
            int y = m_row * getGridHeight();
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.pop_back();
                currentArrow.emplace_back(ConnectionPoint{QPoint(x, y), nullptr, ConnectionPoint::no});
                return true;
            }

            x = m_column * getGridWidth();
            y = m_row * getGridHeight() + getGridHeight() / 2;
            if( (x - mousePos.x()) * (x - mousePos.x()) +
                (y - mousePos.y()) * (y - mousePos.y()) <= CIRCLE_RADIUS * CIRCLE_RADIUS)
            {
                currentArrow.pop_back();
                currentArrow.emplace_back(ConnectionPoint{QPoint(x, y), nullptr, ConnectionPoint::no});
                return true;
            }
            break;
    }
    return false;
}

bool DiagramElement::onElementCollision(const QPoint &mousePos) const
{
    int x = m_column * getGridWidth();
    int y = m_row * getGridHeight();

    QPainterPath path;
    switch(m_type)
    {
        case ToolBoxModel::Element::Function:
        {
            path.addRoundedRect(QRect(x, y, getGridWidth(), getGridHeight()), getGridWidth() / 4, getGridHeight() / 2);
            break;
        }
        case ToolBoxModel::Element::Block:
        {
            return QRect(x, y, getGridWidth(), getGridHeight()).contains(mousePos);
        }
        case ToolBoxModel::Element::If:
        {
            path.moveTo(x + getGridWidth() / 2, y);
            path.lineTo(x + getGridWidth(), y + getGridHeight() / 2);
            path.lineTo(x + getGridWidth() / 2, y + getGridHeight());
            path.lineTo(x, y + getGridHeight() / 2);
            break;
        }
        case ToolBoxModel::Element::For:
        {
            path.moveTo(x, y + getGridHeight() / 2);
            path.lineTo(x + getGridWidth()  / 4, y);
            path.lineTo(x + getGridWidth()  / 1.33, y);
            path.lineTo(x + getGridWidth() , y + getGridHeight() / 2);
            path.lineTo(x + getGridWidth()  / 1.33, y + getGridHeight());
            path.lineTo(x + getGridWidth()  / 4, y + getGridHeight());
            break;
        }
    }
    return path.contains(mousePos);
}

Connector DiagramElement::createCircles() const
{
    int leftPointX  = m_column * getGridWidth();
    int midPointX  = m_column * getGridWidth() + getGridWidth() / 2;
    int rightPointX = m_column * getGridWidth() + getGridWidth();

    int midPointY  = m_row * getGridHeight() + getGridHeight() / 2 ;
    int downPointY = m_row * getGridHeight() + getGridHeight();

    switch (m_type)
    {
        case ToolBoxModel::Element::Function:
        case ToolBoxModel::Element::Block:
            return {{QPoint(midPointX, downPointY), const_cast<DiagramElement*>(this), ConnectionPoint::output}};

        case ToolBoxModel::Element::If:
            return {{QPoint(leftPointX, midPointY), const_cast<DiagramElement*>(this), ConnectionPoint::no},
                    {QPoint(midPointX, downPointY),  const_cast<DiagramElement*>(this), ConnectionPoint::yes},
                    {QPoint(rightPointX, midPointY),  const_cast<DiagramElement*>(this), ConnectionPoint::no}};

        case ToolBoxModel::Element::For:
            return {{QPoint(midPointX, downPointY), const_cast<DiagramElement*>(this), ConnectionPoint::for_body},
                    {QPoint(rightPointX, midPointY),  const_cast<DiagramElement*>(this), ConnectionPoint::output}};
    }
    return {};
}

ConnectionPoint DiagramElement::onCircleCollision(const QPoint mousePos) const
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
    return ConnectionPoint();
}
