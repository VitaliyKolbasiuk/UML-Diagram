#pragma once

#include "ToolBoxModel.h"

#include <QPainter>
#include <QPainterPath>

inline QPainter* gCurrentPainter = nullptr;

template<typename T>
void drawElement(T, int width, int height, int xOffset, int yOffset);

template<>
inline void drawElement<ToolBoxModel::Block>(ToolBoxModel::Block, int width, int height, int xOffset, int yOffset)
{
    gCurrentPainter->drawRect(xOffset, yOffset, width, height);
}

template<>
inline void drawElement<ToolBoxModel::If>(ToolBoxModel::If,
                                          int width, int height,
                                          int xOffset, int yOffset)
{
    QPainterPath path;
    path.moveTo(xOffset + width / 2, yOffset);

    path.lineTo(xOffset + width, yOffset + height / 2);
    path.lineTo(xOffset + width / 2, yOffset + height);
    path.lineTo(xOffset, yOffset + height / 2);

    path.closeSubpath(); // Close the path

    //gCurrentPainter->fillPath(path, gCurrentPainter->pen().color());
    gCurrentPainter->drawPath(path);
}


template<>
inline void drawElement<ToolBoxModel::For>(ToolBoxModel::For,
                                                     int width, int height,
                                                     int xOffset, int yOffset)
{
    QPainterPath path;
    path.moveTo(xOffset, yOffset + height / 2);

    path.lineTo(xOffset + width / 4, yOffset);
    path.lineTo(xOffset + width / 1.33, yOffset);
    path.lineTo(xOffset + width, yOffset + height / 2);
    path.lineTo(xOffset + width / 1.33, yOffset + height);
    path.lineTo(xOffset + width / 4, yOffset + height);
    path.lineTo(xOffset, yOffset + height / 2);

    path.closeSubpath();

    //gCurrentPainter->fillPath(path, gCurrentPainter->pen().color());
    gCurrentPainter->drawPath(path);
}

template<>
inline void drawElement<ToolBoxModel::Select>(ToolBoxModel::Select,
                                           int width, int height,
                                           int xOffset, int yOffset)
{
    //painter->drawRect(xOffset, yOffset, width, height);
}

template<>
inline void drawElement<ToolBoxModel::Connect>(ToolBoxModel::Connect,
                                           int width, int height,
                                           int xOffset, int yOffset)
{
    //painter->drawRect(xOffset, yOffset, width, height);
}

