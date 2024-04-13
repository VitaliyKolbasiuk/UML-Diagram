#pragma once

#include <QMainWindow>
#include <filesystem>
#include <cereal/archives/binary.hpp>

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
    std::string m_fileName = "settings.bin";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void outputCode(const QString& code);

private:
    void initToolBox();
    void saveDiagram();
    void loadDiagram();
    void saveConnectors(cereal::BinaryOutputArchive& archive);
    void loadConnectors(cereal::BinaryInputArchive& archive);
};
