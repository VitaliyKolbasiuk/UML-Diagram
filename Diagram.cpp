#include "Diagram.h"
#include "DrawElements.h"
#include "ToolBoxModel.h"
#include "Generator.h"
#include "ElementText.h"

#include <QMouseEvent>
#include <QTextDocument>
#include <sstream>
#include <string>

Diagram::Diagram()
{
    setMinimumSize(getDiagramWidth() + 10, getDiagramHeight() + 10);

    setMouseTracking(true);
    QWidget::setFocusPolicy(Qt::ClickFocus);

}

void Diagram::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::cyan);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    // Draw grid
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
        QFontMetrics fm(painter.font());
        QStringList lines = QString::fromStdString(element.m_text).split("\n");
        int maxTextWidth = 0;

        for (const QString& line : lines)
        {
            int textWidth = fm.horizontalAdvance(line);
            textWidth = textWidth < 0 ? 3 : textWidth;
            maxTextWidth = std::max(textWidth, maxTextWidth);
        }

        int xOffset = element.m_column * getGridWidth() + (getGridWidth() - maxTextWidth) / 2;
        int yOffset = element.m_row * getGridHeight() + getGridHeight() / 2;

        for (const QString& line : lines)
        {
            painter.drawText(QPoint(xOffset, yOffset), line);
            yOffset += fm.height();
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
        painter.drawPath(path);
        update();
    }

    for (const auto& arrow : m_connectors)
    {
        assert(!arrow.empty());

        if (&arrow == m_selectedConnector)
        {
            continue;
        }

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

    pen.setColor(Qt::gray);
    painter.setPen(pen);
    if (m_selectedConnector != nullptr)
    {
        drawSelectedConnector(&painter);
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

void Diagram::drawSelectedConnector(QPainter *painter)
{
    // Draw the connector

    QPainterPath connectorPath;
    for (int i = 1; i < m_selectedConnector->size(); ++i)
    {
        connectorPath.moveTo((*m_selectedConnector)[i - 1].m_point);
        connectorPath.lineTo((*m_selectedConnector)[i].m_point);
    }
    painter->drawPath(connectorPath);
    connectorPath.closeSubpath();

    // Draw filled circles
    for (int i = 0; i < m_selectedConnector->size() - 1; ++i)
    {
        if ((*m_selectedConnector)[i].m_point.x() - (*m_selectedConnector)[i + 1].m_point.x() == 0)
        {
            int yMid = ((*m_selectedConnector)[i].m_point.y() + (*m_selectedConnector)[i + 1].m_point.y()) / 2;
            painter->drawEllipse(QPoint((*m_selectedConnector)[i].m_point.x(), yMid), 5, 5);
        }
        else
        {
            int xMid = ((*m_selectedConnector)[i].m_point.x() + (*m_selectedConnector)[i + 1].m_point.x()) / 2;
            painter->drawEllipse(QPoint(xMid, (*m_selectedConnector)[i].m_point.y()), 5, 5);
        }
    }
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
        m_currentArrow.erase(m_currentArrow.begin() + 1, m_currentArrow.end());

        QPoint outputPoint = m_currentArrow.front().m_point;
        Connector connectors;
        int yDiff = event->pos().y() - outputPoint.y();
        // Input circle is left or right (IF or FOR)
        if (m_currentArrow.front().m_point.y() != m_currentArrow.front().m_element->m_row * getGridHeight() + getGridHeight())
        {
            bool isLeft = m_currentArrow.front().m_point.x() == m_currentArrow.front().m_element->m_column * getGridWidth();
            if(yDiff < 0 && event->pos().x() / getGridWidth() == m_currentArrow.front().m_element->m_column)
            {
                int x = outputPoint.x();
                if (isLeft)
                {
                    x -= getGridWidth() / 2;
                }
                else
                {
                    x += getGridWidth() / 2;
                }
                connectors.emplace_back(ConnectionPoint{QPoint(x, centerRow(outputPoint.y()))});
                connectors.emplace_back(ConnectionPoint{QPoint(x, centerRow(event->pos().y()))});
                connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
            }
            else if (isLeft && event->pos().x() > outputPoint.x())
            {
                connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() - getGridWidth() / 2, outputPoint.y())});
                if (event->pos().y() / getGridHeight() == m_currentArrow.front().m_element->m_row)
                {
                    connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() - getGridWidth() / 2, centerRow(event->pos().y() - getGridHeight()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y() - getGridHeight()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
                }
                else
                {
                    connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() - getGridWidth() / 2, centerRow(event->pos().y()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
                }
            }
            else if (!isLeft && event->pos().x() < outputPoint.x())
            {
                connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() + getGridWidth() / 2, outputPoint.y())});
                if (event->pos().y() / getGridHeight() == m_currentArrow.front().m_element->m_row)
                {
                    connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() + getGridWidth() / 2, centerRow(event->pos().y() - getGridHeight()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y() - getGridHeight()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
                }
                else
                {
                    connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x() + getGridWidth() / 2, centerRow(event->pos().y()))});
                    connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
                }
            }
            else
            {
                connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), m_currentArrow.front().m_point.y())});
                connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y()))});
            }
        }
        else if (yDiff < 0) // Cursor is above current element
        {
            connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x(), outputPoint.y() + getGridHeight() / 2), nullptr});
            // Cursor row equals to element's row
            if (event->pos().x() / getGridWidth() == m_currentArrow.front().m_element->m_column)
            {
                int x = outputPoint.x();
                if ((event->pos().x() - getGridWidth() / 2) / getGridWidth() != m_currentArrow.front().m_element->m_column)
                {
                    x -= getGridWidth();
                }
                else
                {
                    x += getGridWidth();
                }
                connectors.emplace_back(ConnectionPoint{QPoint(x, outputPoint.y() + getGridHeight() / 2), nullptr});
                connectors.emplace_back(ConnectionPoint{QPoint(x, centerRow(event->pos().y())), nullptr});
            }
            else
            {
                connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(outputPoint.y())), nullptr});
            }
            connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(event->pos().y())), nullptr});
        }
        else
        {
            int yMid = yDiff >= 0 ? yDiff / 2 + outputPoint.y(): outputPoint.y();
            connectors.emplace_back(ConnectionPoint{QPoint(outputPoint.x(), centerRow(yMid)), nullptr});
            connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(event->pos().x()), centerRow(yMid)), nullptr});
            connectors.emplace_back(ConnectionPoint{QPoint(centerColumn(m_newArrowPoint.x()), centerRow(m_newArrowPoint.y())), nullptr});
        }
        for (const auto& connector : connectors)
        {
            m_currentArrow.emplace_back(connector);
        }
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

int Diagram::centerColumn(int x)
{
    return x / getGridWidth() * getGridWidth() + getGridWidth() / 2;
}

int Diagram::centerRow(int y)
{
    return y / getGridHeight() * getGridHeight() + getGridHeight() / 2;
}

void Diagram::mouseReleaseEvent(QMouseEvent* event)
{
    m_selectedConnector = onConnectorCollision(event->pos());
    update();
}

Connector* Diagram::onConnectorCollision(const QPoint mousePos)
{
    for (auto& connector : m_connectors)
    {
        for (int i = 0; i < connector.size() - 1; ++i)
        {
            if (connector[i].m_point.x() - connector[i + 1].m_point.x() == 0)
            {
                int yMin = std::min(connector[i].m_point.y(), connector[i + 1].m_point.y());
                int yMax = std::max(connector[i].m_point.y(), connector[i + 1].m_point.y());
                //qDebug() << std::abs(mousePos.x() - connector[i].m_point.x());
                if (std::abs(mousePos.x() - connector[i].m_point.x()) < 5 &&
                    mousePos.y() >= yMin && mousePos.y() <= yMax)
                {
                    return &connector;
                }
            }
            else if (connector[i].m_point.y() - connector[i + 1].m_point.y() == 0)
            {
                int xMin = std::min(connector[i].m_point.x(), connector[i + 1].m_point.x());
                int xMax = std::max(connector[i].m_point.x(), connector[i + 1].m_point.x());
                if (std::abs(mousePos.y() - connector[i].m_point.y()) < 5 &&
                    mousePos.x() >= xMin && mousePos.x() <= xMax)
                {
                    return &connector;
                }
            }
        }
    }
    return nullptr;
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
                generateCode();
            }
            return;
        }
    }
}

void Diagram::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        // Delete selected connector
        if (m_selectedConnector != nullptr)
        {
            deleteSelectedConnector();
        }

        // Delete current element
        if (m_currentElement != nullptr && m_currentArrow.empty())
        {
            deleteCurrentElement();
        }

        generateCode();
        update();
    }
}

void Diagram::deleteCurrentElement()
{
    for (auto it = m_diagramElements.begin(); it != m_diagramElements.end(); ++it)
    {
        if (&(*it) == m_currentElement)
        {
            // Find and delete all related connectors
            deleteRelatedConnectors(&(*it));

            m_diagramElements.erase(it);
            m_currentElement = nullptr;
            break;
        }
    }
}

void Diagram::deleteRelatedConnectors(const DiagramElement* element)
{
    for (auto it = m_connectors.begin(); it != m_connectors.end(); ++it)
    {
        if (it->front().m_element == element || it->back().m_element == element)
        {
            m_connectors.erase(it--);
        }
    }
}

void Diagram::deleteSelectedConnector()
{
    for (auto it = m_connectors.begin(); it != m_connectors.end(); ++it)
    {
        if (&(*it) == m_selectedConnector)
        {
            m_connectors.erase(it);
            m_selectedConnector = nullptr;
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

    QWidget::grabKeyboard();
    generateCode();
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
    generate_code::Code code = generate_code::diagramToPythonPseudoCode(*this);
    std::ostringstream oss;
    for(const auto& element : code)
    {
        element->generateCpp(oss, 0);
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



