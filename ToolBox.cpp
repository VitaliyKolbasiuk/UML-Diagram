#include "ToolBox.h"
#include "ToolBoxModel.h"
#include "DrawElements.h"
#include "DragObject.h"
#include "Diagram.h"

#include <QMouseEvent>

#define ELEMENT_WIDTH  200
#define ELEMENT_HEIGHT 100
#define PADDING        20

ToolBox::ToolBox()
{
    setMaximumSize(ELEMENT_WIDTH, (ELEMENT_HEIGHT + PADDING) * ToolBoxModel::gModel.m_elements.size() + PADDING);
    setMinimumSize(ELEMENT_WIDTH, (ELEMENT_HEIGHT + PADDING) * ToolBoxModel::gModel.m_elements.size() + PADDING);

    setMouseTracking(true);
}

void ToolBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QPen pen(Qt::yellow);
    pen.setWidth(4);
    painter.setPen(pen);

    double xScale = 0.75, yScale = 0.75;
    painter.scale(xScale, yScale);

    gCurrentPainter = &painter;
    int yOffset = PADDING;
    for (const auto& element: ToolBoxModel::gModel.m_elements)
    {
        element->draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, 0, yOffset);
        yOffset += ELEMENT_HEIGHT + PADDING;
        //resize(ELEMENT_WIDTH, yOffset);
    }

}

void ToolBox::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "ToolBox mouse pressed";
    m_dragObject->setVisible(true);

    int index = event->pos().y() / ELEMENT_HEIGHT;
    m_dragObject->setDragObjectType(ToolBoxModel::gModel.m_elements[index]->type());
    m_dragObject->move(event->pos());
}

void ToolBox::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "ToolBox mouse move" << event->pos().x() << event->pos().y();
    if (m_dragObject->isVisible())
    {
        m_dragObject->move(event->pos() - QPoint(ELEMENT_WIDTH / 2, ELEMENT_HEIGHT / 2));
        if (event->pos().x() - width() > 0)
        {
            m_diagram->setCurrentElement(m_dragObject->objectType());
        }
    }
}

void ToolBox::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "ToolBox mouse release";
    m_dragObject->setVisible(false);

    m_diagram->currentItemReleased();
}

void ToolBox::setDragObjectRef(DragObject &dragObject)
{
    m_dragObject = &dragObject;
}

void ToolBox::setDiagramRef(QWidget &diagram)
{
    m_diagram = reinterpret_cast<Diagram*>(&diagram);
}

