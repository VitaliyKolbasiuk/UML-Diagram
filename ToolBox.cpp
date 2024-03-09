#include "ToolBox.h"
#include "ToolBoxModel.h"
#include "DrawElements.h"

#define ELEMENT_HEIGHT 100
#define ELEMENT_WIDTH  200
#define PADDING        20

ToolBox::ToolBox()
{
    setMinimumSize(ELEMENT_WIDTH, (ELEMENT_HEIGHT + PADDING) * ToolBoxModel::gModel.m_elements.size() + PADDING);
}

void ToolBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QPen pen(Qt::yellow);

    pen.setWidth(4);
    painter.setPen(pen);

    int yOffset = PADDING;
    gCurrentPainter = &painter;
    for (const auto& element: ToolBoxModel::gModel.m_elements)
    {
        element->draw(ELEMENT_WIDTH, ELEMENT_HEIGHT, 0, yOffset);
        yOffset += ELEMENT_HEIGHT + PADDING;
        resize(ELEMENT_WIDTH, yOffset);
    }
}