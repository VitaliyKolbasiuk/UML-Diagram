#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ToolBoxModel.h"
#include "DragObject.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("background-color: black;");
    m_dragObejct = new DragObject(this);
    ui->m_toolBox->setDragObjectRef(*m_dragObejct);
    ui->m_toolBox->setDiagramRef(*ui->m_scrollDiagram->widget());
    initToolBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBox()
{
}
