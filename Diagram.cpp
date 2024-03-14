#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"

#include <QPainterPath>

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

    if (m_currGridElement)
    {
        drawGridElement(&painter, *m_currGridElement);
        update();
    }

    for (const auto& element : m_gridElements)
    {
        drawGridElement(&painter, element);
    }
}

void Diagram::drawGrid(QPainter* painter)
{
    QPainterPath path;

    painter->drawPath(path);
    for (int x = 0; x <= getDiagramWidth(); x += getGridWidth())
    {
        path.moveTo(x, 0);
        path.lineTo(x, getDiagramHeight());
    }
    for (int y = 0; y <= getDiagramHeight(); y += getGridHeight())
    {
        path.moveTo(0, y);
        path.lineTo(getDiagramWidth(), y);
    }
    painter->drawPath(path);

}

void Diagram::drawGridElement(QPainter *painter, const GridElement& element)
{
    QPen pen(Qt::yellow);
    painter->setPen(pen);
    gCurrentPainter = painter;
    switch (element.type)
    {
        case ToolBoxModel::Element::Block:
        {
            ToolBoxModel::Block elementBlock;
            elementBlock.draw(getGridWidth(), getGridHeight(), element.xOffset, element.yOffset);
            break;
        }
        case ToolBoxModel::Element::If:
        {
            ToolBoxModel::If elementIf;
            elementIf.draw(getGridWidth(), getGridHeight(), element.xOffset, element.yOffset);
            break;
        }
        case ToolBoxModel::Element::For:
        {
            ToolBoxModel::For elementFor;
            elementFor.draw(getGridWidth(), getGridHeight(), element.xOffset, element.yOffset);
            break;
        }
    }
}

void Diagram::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "Diagram mouse pressed";
}

void Diagram::mouseMoveEvent(QMouseEvent* event)
{
    //qDebug() << "Diagram mouse move";
}

void Diagram::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << "Diagram mouse release";
}

void Diagram::setCurrentElement(ToolBoxModel::Element::Type type, int xOffset, int yOffset)
{
    if (!m_currGridElement)
    {
        m_currGridElement = new GridElement(type, xOffset, yOffset);
    }
    else
    {
        m_currGridElement->xOffset = xOffset;
        m_currGridElement->yOffset = yOffset;
    }
}

void Diagram::currentItemReleased(int xOffset, int yOffset)
{
    int x = mapFromGlobal(QCursor::pos()).x();
    if (x > 0 && x < width())
    {
        m_currGridElement->xOffset = xOffset;
        m_currGridElement->yOffset = yOffset;
        m_gridElements.emplace_back(*m_currGridElement);
    }
    delete m_currGridElement;
    m_currGridElement = nullptr;
}
