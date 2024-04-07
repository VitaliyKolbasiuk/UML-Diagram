#include "ElementText.h"
#include "ui_ElementText.h"

#include <QPushButton>

ElementText::ElementText(QWidget *parent) :
        QDialog(parent), ui(new Ui::ElementText)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ElementText::onCancelBtnRelease);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ElementText::onSaveBtnRelease);
}

ElementText::~ElementText()
{
    delete ui;
}

void ElementText::onCancelBtnRelease()
{
    reject();
}

void ElementText::onSaveBtnRelease()
{
    accept();
}

QString ElementText::getText() const
{
    return ui->m_textEdit->toPlainText();
}