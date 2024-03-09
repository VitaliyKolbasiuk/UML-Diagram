#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ToolBoxModel.h"
#include "DragObject.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dragObejct = new DragObject(this);
    ui->m_toolBox->setDragObjectRef(*m_dragObejct);
    initToolBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBox()
{
}
