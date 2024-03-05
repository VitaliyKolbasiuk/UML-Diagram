#pragma once

#include <QWidget>
#include <QSplitter>

class Diagram : public QWidget
{

public:
    Diagram(QSplitter* parent) : QWidget(parent) {}
};
