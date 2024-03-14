#pragma once

#include <QFrame>

#include "ToolBoxModel.h"

class DragObject : public QFrame
{
Q_OBJECT

ToolBoxModel::Element::Type  m_objectType;

public:
    explicit DragObject(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent* event) override;

    void setDragObjectType(ToolBoxModel::Element::Type type);
    ToolBoxModel::Element::Type objectType();
private:

};