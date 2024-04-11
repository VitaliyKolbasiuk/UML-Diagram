#pragma once

#include "ToolBoxModel.h"
#include "Utils.h"

#include <QString>
#include <QPoint>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

class DiagramElement
{
public:
    ToolBoxModel::Element::Type m_type = ToolBoxModel::Element::Block;
    int m_column;
    int m_row;
    std::string m_text;

    bool onInputCircleCollision(const QPoint& mousePos, Connector& connector) const;
    bool onElementCollision(const QPoint& mousePos) const;
    Connector createCircles() const;
    ConnectionPoint onCircleCollision(const QPoint mousePos) const;

    template <class Archive>
    void save( Archive & ar ) const
    {
        ar( (int)m_type );
        ar( m_column );
        ar( m_row );
        ar( m_text );
    }

    template <class Archive>
    void load( Archive & ar )
    {
        int type;
        ar( type );
        m_type = static_cast<ToolBoxModel::Element::Type>(type);
        ar( m_column );
        ar( m_row );
        ar( m_text );
    }
};
