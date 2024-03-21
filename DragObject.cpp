#include "DragObject.h"
#include "DrawElements.h"

#include <QPainter>
#include <QPainterPath>
#include <QCursor>

#define ELEMENT_WIDTH   200
#define ELEMENT_HEIGHT  100

DragObject::DragObject(QWidget *parent) :
        QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setVisible(false);
    setStyleSheet("background-color: rgba(0, 0, 0, 0);");

    resize(ELEMENT_WIDTH * 2, ELEMENT_HEIGHT * 2);
}

void DragObject::paintEvent(QPaintEvent *event)
{
    if (isVisible())
    {
        QPainter painter(this);
        QPen     pen;
        pen.setColor(Qt::yellow);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);

        float xScale = 0.5, yScale = 0.5;
        painter.scale(xScale, yScale);

        gCurrentPainter = &painter;

        QPoint localPos = mapFromGlobal(QCursor::pos());

        switch(m_objectType)
        {
            case ToolBoxModel::Element::Function:
                ToolBoxModel::Function{}.draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, localPos.x(), localPos.y());
                break;

            case ToolBoxModel::Element::Block:
                ToolBoxModel::Block{}.draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, localPos.x(), localPos.y());
                break;

            case ToolBoxModel::Element::If:
                ToolBoxModel::If{}.draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, localPos.x(), localPos.y());
                break;

            case ToolBoxModel::Element::For:
                ToolBoxModel::For{}.draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, localPos.x(), localPos.y());
                break;
        }
    }
}

void DragObject::setDragObjectType(ToolBoxModel::Element::Type type)
{
    m_objectType = type;
}

ToolBoxModel::Element::Type DragObject::objectType()
{
    return m_objectType;
}
