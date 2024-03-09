#pragma once

#include <QWidget>
#include <QSplitter>
#include "QPainter"

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

class Diagram : public QWidget
{


public:
    Diagram(QSplitter* parent) : QWidget(parent) {}

    void paintEvent(QPaintEvent* event) override;

private:

    void drawGrid(QPainter* painter);
};
