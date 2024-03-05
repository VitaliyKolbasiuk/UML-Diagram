#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

class ToolBox : public QWidget
{

public:
    ToolBox();
    void paintEvent(QPaintEvent * event) override;
};
