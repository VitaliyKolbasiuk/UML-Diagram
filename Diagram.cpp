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

    pen.setColor(Qt::yellow);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    gCurrentPainter = &painter;
    if (m_dragElement)
    {
        drawGridElement(*m_dragElement);
        update();
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
        drawCurrentElement();
        update();
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

void Diagram::drawGridElement(const DiagramElement& element)
{
    switch (element.type)
    {
        case ToolBoxModel::Element::Block:
        {
            ToolBoxModel::Block elementBlock;
            elementBlock.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;
        }
        case ToolBoxModel::Element::If:
        {
            ToolBoxModel::If elementIf;
            elementIf.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;
        }
        case ToolBoxModel::Element::For:
        {
            ToolBoxModel::For elementFor;
            elementFor.draw(getGridWidth(), getGridHeight(), element.column * getGridWidth(), element.row * getGridHeight());
            break;
        }
    }
}

void Diagram::drawCurrentElement()
{
    QPainter painter(this);
    QBrush brush(Qt::darkGray);
    painter.setBrush(brush);
    gCurrentPainter = &painter;

    drawGridElement(*m_currentElement);

    switch (m_currentElement->type)
    {
        case ToolBoxModel::Element::Block:
            painter.drawEllipse(m_currentElement->column * getGridWidth() + getGridWidth() / 2,
                                m_currentElement->row * getGridHeight() + getGridHeight() - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);
            break;

        case ToolBoxModel::Element::If:
            painter.drawEllipse(m_currentElement->column * getGridWidth(),
                                m_currentElement->row * getGridHeight() + getGridHeight() / 2 - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);

            painter.drawEllipse(m_currentElement->column * getGridWidth() + getGridWidth() / 2 - getGridHeight() / 16,
                                m_currentElement->row * getGridHeight() + getGridHeight() - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);

            painter.drawEllipse(m_currentElement->column * getGridWidth() + getGridWidth() - getGridHeight() / 16,
                                m_currentElement->row * getGridHeight() + getGridHeight() / 2 - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);
            break;

        case ToolBoxModel::Element::For:
            painter.drawEllipse(m_currentElement->column * getGridWidth() + getGridWidth() / 2,
                                m_currentElement->row * getGridHeight() + getGridHeight() - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);

            painter.drawEllipse(m_currentElement->column * getGridWidth() + getGridWidth() - getGridHeight() / 16,
                                m_currentElement->row * getGridHeight() + getGridHeight() / 2 - getGridHeight() / 16,
                                getGridHeight() / 8, getGridHeight() / 8);
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
    if (mousePos.x() >= getDiagramWidth() || mousePos.y() >= getDiagramHeight())
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
        qDebug() << i << j;
    }
}

void Diagram::currentItemReleased()
{
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    if (mousePos.x() >= 0 && mousePos.x() <= getDiagramWidth() && mousePos.y() >= 0 && mousePos.y() <= getDiagramHeight())
    {
        int i = mousePos.x() / getGridWidth();
        int j = mousePos.y()  / getGridHeight();

        for(const auto& element : m_diagramElements)
        {
            if (element.column == i && element.row == j)
            {
                delete m_dragElement;
                m_dragElement = nullptr;
                return;
            }
        }

        m_dragElement->column = i;
        m_dragElement->row = j;
        m_diagramElements.emplace_back(*m_dragElement);
    }
    delete m_dragElement;
    m_dragElement = nullptr;
}
