#pragma once

#include "Utils.h"

#include <QPoint>
#include <vector>

class DiagramElement;
class ConnectionPoint;

struct ConnectionPoint
{
    QPoint m_point;
    DiagramElement* m_element = nullptr;
    enum {input, yes, no, for_body, output} m_type = output;

};
