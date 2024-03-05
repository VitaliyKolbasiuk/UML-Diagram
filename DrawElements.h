#pragma once

#include <QPainter>

template<typename T>
void drawElement(T, QPainter* painter, int width, int height, int xOffset, int yOffset);

template<>
void drawElement<std::shared_ptr<ToolBoxModel::Element> >(std::shared_ptr<ToolBoxModel::Element>, QPainter* painter,  int width, int height, int xOffset, int yOffset)
{
    painter->drawRect(xOffset, yOffset, width, height);
}
