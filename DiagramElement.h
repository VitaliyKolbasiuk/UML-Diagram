#pragma once

#include "ToolBoxModel.h"
#include "Utils.h"

#include <QString>
#include <QPoint>

class DiagramElement
{
public:
    ToolBoxModel::Element::Type m_type;
    int m_column;
    int m_row;
    QString m_text;

    bool onInputCircleCollision(const QPoint& mousePos, Connector& connector) const;
    bool onElementCollision(const QPoint& mousePos) const;
    Connector createCircles() const;
    ConnectionPoint onCircleCollision(const QPoint mousePos) const;
};
