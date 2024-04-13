#include <fstream>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

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
    ui->m_textEdit->setMaximumWidth(ui->m_scrollDiagram->widget()->size().width() - 20);
    m_dragObejct = new DragObject(this);
    ui->m_toolBox->setDragObjectRef(*m_dragObejct);
    ui->m_toolBox->setDiagramRef(*ui->m_scrollDiagram->widget());
    initToolBox();

    connect(ui->m_save, &QPushButton::released, this, &MainWindow::saveDiagram);
    connect(ui->m_load, &QPushButton::released, this, &MainWindow::loadDiagram);
    connect(static_cast<Diagram*>(ui->m_scrollDiagram->widget()), &Diagram::codeGenerated, this, &MainWindow::outputCode);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBox()
{
}

void MainWindow::outputCode(const QString& code)
{
    ui->m_textEdit->setText(code);
}

void MainWindow::saveDiagram()
{
    std::ofstream os(m_fileName, std::ios::binary);
    cereal::BinaryOutputArchive archive( os );
    Diagram* diagram = ui->m_toolBox->getDiagram();
    archive(diagram->m_diagramElements);
    saveConnectors(archive);
}

void MainWindow::loadDiagram()
{
    std::ifstream ios(m_fileName, std::ios::binary);
    cereal::BinaryInputArchive archive( ios );
    Diagram* diagram = ui->m_toolBox->getDiagram();
    archive(diagram->m_diagramElements);
    loadConnectors(archive);
    diagram->updateDiagram();
}

void MainWindow::saveConnectors(cereal::BinaryOutputArchive& archive)
{
    Diagram* diagram = ui->m_toolBox->getDiagram();
    size_t size = diagram->m_connectors.size();
    archive( size );
    for (const auto& connector : diagram->m_connectors)
    {
        size = connector.size();
        archive(size);
        for (const auto& point : connector)
        {
            archive (point.m_point.x());
            archive (point.m_point.y());
            archive (static_cast<int>(point.m_type));
            int index = -1;
            for (int i = 0; i < diagram->m_diagramElements.size(); ++i)
            {
                if (point.m_element == &diagram->m_diagramElements[i])
                {
                    index = i;
                    break;
                }
            }
            archive (index);
        }
    }
}

void MainWindow::loadConnectors(cereal::BinaryInputArchive& archive)
{
    Diagram* diagram = ui->m_toolBox->getDiagram();

    size_t size;
    archive( size );
    for (int i = 0; i < size; ++i)
    {
        size_t connectorSize;
        archive ( connectorSize);
        Connector connector;
        for (int j = 0; j < connectorSize; ++j)
        {
            int x, y;
            archive (x);
            archive (y);

            int type;
            archive (type);

            int elementIndex;
            archive(elementIndex);
            if (elementIndex != -1)
            {
                connector.emplace_back(ConnectionPoint{QPoint(x, y), &diagram->m_diagramElements[elementIndex], static_cast<ConnectionPoint::type>(type)});
            }
        }
        diagram->m_connectors.emplace_back(connector);
    }
}
