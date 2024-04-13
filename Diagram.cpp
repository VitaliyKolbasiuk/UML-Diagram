#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"
#include "Python.h"
#include "ElementText.h"

#include <QMouseEvent>
#include <sstream>
#include <string>

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

        path.moveTo(m_currentArrow.front().m_point);
        for (int i = 1; i < m_currentArrow.size(); ++i)
        {
            path.lineTo(m_currentArrow[i].m_point);
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
            path.moveTo(arrow[i - 1].m_point);
            path.lineTo(arrow[i].m_point);
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
    switch (element.m_type)
    {
        case ToolBoxModel::Element::Function:
            ToolBoxModel::Function{}.draw(getGridWidth(), getGridHeight(), element.m_column * getGridWidth(), element.m_row * getGridHeight());
            break;

        case ToolBoxModel::Element::Block:
            ToolBoxModel::Block{}.draw(getGridWidth(), getGridHeight(), element.m_column * getGridWidth(), element.m_row * getGridHeight());
            break;

        case ToolBoxModel::Element::If:
            ToolBoxModel::If{}.draw(getGridWidth(), getGridHeight(), element.m_column * getGridWidth(), element.m_row * getGridHeight());
            break;

        case ToolBoxModel::Element::For:
            ToolBoxModel::For{}.draw(getGridWidth(), getGridHeight(), element.m_column * getGridWidth(), element.m_row * getGridHeight());
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

    drawCurrElementCircles(painter, m_currentElement->createCircles());
}

void Diagram::mousePressEvent(QMouseEvent* event)
{
    ConnectionPoint connectionPoint;
    if (m_currentElement != nullptr)
    {
        connectionPoint = m_currentElement->onCircleCollision(event->pos());
    }

    if (!connectionPoint.m_point.isNull())
    {
        m_currentArrow.emplace_back(connectionPoint);
        m_newArrowPoint = connectionPoint.m_point;
    }
    else if (m_inputElement != nullptr && m_inputElement->onInputCircleCollision(event->pos(), m_currentArrow))
    {
        m_currentArrow.back().m_element = m_inputElement;
        m_currentArrow.back().m_type = ConnectionPoint::input;
        m_connectors.emplace_back(m_currentArrow);

        m_currentArrow.clear();
        m_inputElement = nullptr;
        m_currentElement = nullptr;
        m_newArrowPoint = QPoint();

        generateCode();

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
        if (element.m_column == i && element.m_row == j)
        {
            return &element;
        }
    }
    return nullptr;
}

void Diagram::mouseMoveEvent(QMouseEvent* event)
{
    DiagramElement* elementPtr = onElementClicked(event->pos());

    ConnectionPoint connectionPoint;
    if (m_currentElement != nullptr)
    {
        connectionPoint = m_currentElement->onCircleCollision(event->pos());
    }

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

    if (!elementPtr && connectionPoint.m_point.isNull() && !m_inputElement)
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

void Diagram::mouseDoubleClickEvent(QMouseEvent *event)
{
    for (auto& element : m_diagramElements)
    {
        if (element.onElementCollision(event->pos()))
        {
            ElementText elementText(this);
            if (elementText.exec() == QDialog::Accepted)
            {
                element.m_text = elementText.getText().toStdString();
            }
            return;
        }
    }
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
        m_dragElement->m_column = i;
        m_dragElement->m_row = j;
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
        if (element.m_column == i && element.m_row == j)
        {
            deleteDragElement();
            return;
        }
    }

    m_dragElement->m_column = i;
    m_dragElement->m_row = j;
    m_diagramElements.emplace_back(*m_dragElement);
    deleteDragElement();
}

bool Diagram::isWithinDiagramArea(const QPoint point)
{
    return point.x() >= 0 && point.x() < getDiagramWidth() &&
           point.y() >= 0 && point.y() < getDiagramHeight();
}

void Diagram::deleteDragElement()
{
    delete m_dragElement;
    m_dragElement = nullptr;
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
    drawCurrElementCircles(painter, createInputCircles());
}

Connector Diagram::createInputCircles() const
{
    int leftPointX  = m_inputElement->m_column * getGridWidth();
    int midPointX  = m_inputElement->m_column * getGridWidth() + getGridWidth() / 2;

    int topPointY = m_inputElement->m_row * getGridHeight();
    int midPointY  = m_inputElement->m_row * getGridHeight() + getGridHeight() / 2 ;
    switch(m_inputElement->m_type)
    {
        case ToolBoxModel::Element::Function:
            return {};
        case ToolBoxModel::Element::Block:
        case ToolBoxModel::Element::If:
            return {{QPoint(midPointX, topPointY), m_inputElement, ConnectionPoint::input}};

        case ToolBoxModel::Element::For:
            return {{QPoint(leftPointX, midPointY), m_inputElement, ConnectionPoint::input},
                    {QPoint(midPointX, topPointY), m_inputElement, ConnectionPoint::input}};
    }
    return {};
}

const DiagramElement* Diagram::findFirstElement() const
{
    const DiagramElement* highestElement = &m_diagramElements.front();
    for (const auto& element : m_diagramElements)
    {
        if (element.m_row < highestElement->m_row)
        {
            return &element;
        }
        else if (element.m_row == highestElement->m_row)
        {
            if (element.m_column < highestElement->m_column)
            {
                return &element;
            }
        }
    }
    return highestElement;
}

void Diagram::updateDiagram()
{
    m_currentArrow.clear();
    m_newArrowPoint = QPoint();
    m_currentElement = nullptr;
    m_inputElement = nullptr;

    generateCode();
    update();
}

void Diagram::generateCode()
{
    python_code::Code pythonCode = python_code::diagramToPythonPseudoCode(*this);
    std::ostringstream oss;
    for(const auto& element : pythonCode)
    {
        element->generate(oss, 0);
    }
    emit codeGenerated(QString::fromStdString(oss.str()));
}

const Connector* Diagram::getOutputConnector(const DiagramElement* element) const
{
    for (const auto& connector : m_connectors)
    {
        if (connector.front().m_element == element)
        {
            if (connector.front().m_type == ConnectionPoint::output)
            {
                return &connector;
            }
            continue;
        }
    }
    return nullptr;
}

const Connector* Diagram::getNoOutputConnector(const DiagramElement* element) const
{
    QPoint leftOutputPoint(element->m_column * getGridWidth(), element->m_row * getGridHeight() + getGridHeight() / 2);
    QPoint rightOutputPoint(element->m_column * getGridWidth() + getGridWidth(), element->m_row * getGridHeight() + getGridHeight() / 2);
    for (const auto& connector : m_connectors)
    {
        if (connector.front().m_point == leftOutputPoint || connector.front().m_point == rightOutputPoint)
        {
            return &connector;
        }
    }
    return nullptr;
}

const Connector* Diagram::getYesOutputConnector(const DiagramElement* element) const
{
    QPoint outputPoint(element->m_column * getGridWidth() + getGridWidth() / 2, element->m_row * getGridHeight() + getGridHeight());
    for (const auto& connector : m_connectors)
    {
        if (connector.front().m_point == outputPoint)
        {
            return &connector;
        }
    }
    return nullptr;
}

const Connector* Diagram::getBodyOutputConnector(const DiagramElement* element) const
{
    QPoint outputPoint(element->m_column * getGridWidth() + getGridWidth() / 2, element->m_row * getGridHeight() + getGridHeight());
    for (const auto& connector : m_connectors)
    {
        if (connector.front().m_point == outputPoint)
        {
            return &connector;
        }
    }
    return nullptr;
}



