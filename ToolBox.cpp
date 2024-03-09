#include "ToolBox.h"
#include "ToolBoxModel.h"
#include "DrawElements.h"
#include "DragObject.h"

#include <QCursor>

#define ELEMENT_HEIGHT 100
#define ELEMENT_WIDTH  200
#define PADDING        20

ToolBox::ToolBox()
{
    setMaximumSize(ELEMENT_WIDTH, (ELEMENT_HEIGHT + PADDING) * ToolBoxModel::gModel.m_elements.size() + PADDING);
    setMinimumSize(ELEMENT_WIDTH, (ELEMENT_HEIGHT + PADDING) * ToolBoxModel::gModel.m_elements.size() + PADDING);
}

void ToolBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QPen pen(Qt::yellow);

    pen.setWidth(4);
    painter.setPen(pen);

    QTransform transform;
    double xScale = 0.75, yScale = 0.75;
    transform.scale(xScale, yScale);
    painter.setTransform(transform);

    int yOffset = PADDING;
    gCurrentPainter = &painter;
    for (const auto& element: ToolBoxModel::gModel.m_elements)
    {
        element->draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, 0, yOffset);
        yOffset += ELEMENT_HEIGHT + PADDING;
        //resize(ELEMENT_WIDTH, yOffset);
    }
}

void ToolBox::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "ToolBox mouse pressed";
    m_dragObject->setVisible(true);
}

void ToolBox::mouseMoveEvent(QMouseEvent *event)
{
    setMouseTracking(true);
    qDebug() << "ToolBox mouse move";
}

void ToolBox::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "ToolBox mouse release";
    m_dragObject->setVisible(false);
}

void ToolBox::setDragObjectRef(DragObject &dragObject)
{
    m_dragObject = &dragObject;
}

