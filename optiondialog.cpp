#include "optiondialog.h"
#include "ui_optiondialog.h"
#include <QColorDialog>

OptionDialog::OptionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
    , currentPart(nullptr)
{
    ui->setupUi(this);
	

	connect(ui->colorButton, &QPushButton::clicked, this, &OptionDialog::onColorButtonClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this, &OptionDialog::onDeleteButtonClicked);
}

OptionDialog::~OptionDialog() {
    delete ui;
}

void OptionDialog::setModelPart(ModelPart *part) {
    if (!part) return;

    currentPart = part;
    ui->lineEdit->setText(part->data(0).toString());

    currentColor = part->color();
	updateColorButton();

    ui->checkBox->setChecked(part->visible());

}

void OptionDialog::accept() {
    if (currentPart) {
        QString name = ui->lineEdit->text();
        bool visible = ui->checkBox->isChecked();

        currentPart->setName(name);
        currentPart->setColor(currentColor);
        currentPart->setVisible(visible);
    }

    QDialog::accept();
}

void OptionDialog::getModelPartData(QString &name, QColor &color, bool &visible) const {
    name = ui->lineEdit->text();
    color = currentColor;
    visible = ui->checkBox->isChecked();
}

void OptionDialog::onDeleteButtonClicked() {
    emit deleteRequested();
    this->reject();
}


void OptionDialog::updateColorButton() {
    QString style = QString("background-color: %1; border: 1px solid black;").arg(currentColor.name());
    ui->colorButton->setStyleSheet(style);
}

void OptionDialog::onColorButtonClicked() {
    QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color");
    if (newColor.isValid()) {
        currentColor = newColor;
        updateColorButton();
    }
}



