#pragma once

#include <vector>

class ConnectionPoint;
using Connector = std::vector<ConnectionPoint>;

#define CIRCLE_RADIUS getGridWidth() / 20

inline int getDiagramWidth()
{
    return 500;
}

inline int getDiagramHeight()
{
    return 600;
}

inline int getGridWidth()
{
    return 100;
}

inline int getGridHeight()
{
    return 50;
}