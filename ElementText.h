#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class ElementText;
}
QT_END_NAMESPACE

class ElementText : public QDialog
{
Q_OBJECT

public:
    explicit ElementText(QWidget *parent = nullptr);

    ~ElementText() override;

public slots:
    void onCancelBtnRelease();
    void onSaveBtnRelease();

public:

    QString getText() const;

private:
    Ui::ElementText *ui;
};
