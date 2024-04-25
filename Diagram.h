#pragma once

#include "ToolBoxModel.h"
#include "Utils.h"
#include "ConnectionPoint.h"
#include "DiagramElement.h"

#include <QWidget>
#include <map>

class Diagram : public QWidget
{
    Q_OBJECT
    friend class MainWindow;

    std::vector<DiagramElement> m_diagramElements;
    std::vector<Connector>      m_connectors;

    DiagramElement*             m_dragElement = nullptr;
    DiagramElement*             m_currentElement = nullptr;
    DiagramElement*             m_inputElement = nullptr;
    Connector*                  m_currentConnector = nullptr;

    Connector                   m_currentArrow;
    QPoint                      m_newArrowPoint;

signals:
    void codeGenerated(const QString& code);

public:
    Diagram();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void setCurrentElement(ToolBoxModel::Element::Type type);
    void currentItemReleased();
    const DiagramElement* findFirstElement() const;
    void updateDiagram();

private:
    static void drawGridElement(const DiagramElement& element);
    static void drawGrid(QPainter* painter);
    DiagramElement* onElementClicked(QPoint mousePos);
    void drawCurrentElement(QPainter* painter);
    void drawSelectedConnector(QPainter* painter);
    static bool isWithinDiagramArea(const QPoint point) ;
    void deleteDragElement();
    Connector createInputCircles() const;
    static void drawCurrElementCircles(QPainter* painter, const std::vector<ConnectionPoint>& points);
    void drawInputElement(QPainter* painter);
    void generateCode();
    int centerColumn(int x);
    int centerRow(int y);
    Connector* onConnectorCollision(const QPoint mousePos);
    void deleteSelectedConnector();

public:
    const Connector* getOutputConnector(const DiagramElement* element) const;
    const Connector* getYesOutputConnector(const DiagramElement* element) const;
    const Connector* getNoOutputConnector(const DiagramElement* element) const;
    const Connector* getBodyOutputConnector(const DiagramElement* element) const;
};
