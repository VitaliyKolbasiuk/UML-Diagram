#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "ToolBoxModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralwidget->setFixedSize(1000, 650);
    ui->scrollArea->setMinimumSize(ui->m_splitter->width() / 2, ui->m_splitter->height());
    initToolBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBox()
{
}
