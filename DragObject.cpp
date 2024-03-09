#include "DragObject.h"


DragObject::DragObject(QWidget *parent) :
        QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setVisible(false);
}