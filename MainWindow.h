#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class DragObject;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow* ui;
    DragObject*     m_dragObejct;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initToolBox();
    void generate();
};
