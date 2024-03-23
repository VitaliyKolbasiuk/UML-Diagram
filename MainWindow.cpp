#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ToolBoxModel.h"
#include "DragObject.h"
#include "Python.h"

#include <QPushButton>

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

    connect(ui->m_generateBtn, &QPushButton::released, this, [this](){
        generate();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBox()
{
}

void MainWindow::generate()
{
    auto diagram = ui->m_toolBox->getDiagram();


}
