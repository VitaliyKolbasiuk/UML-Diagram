#pragma once

#include <QFrame>

class DragObject : public QFrame
{
Q_OBJECT

public:
    explicit DragObject(QWidget *parent = nullptr);

    void setDragObjectType();
private:

};