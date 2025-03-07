#include "Option_Dialog.h"
#include "ui_Option_Dialog.h"

Option_Dialog::Option_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Option_Dialog)
    , currentPart(nullptr)
{
    ui->setupUi(this);

    connect(ui->RedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Option_Dialog::updateColorPreview);
    connect(ui->GreenSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Option_Dialog::updateColorPreview);
    connect(ui->BlueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Option_Dialog::updateColorPreview);
}

Option_Dialog::~Option_Dialog() {
    delete ui;
}

void Option_Dialog::setModelPart(ModelPart *part) {
    if (!part) return;

    currentPart = part;
    ui->lineEdit->setText(part->data(0).toString());

    QColor color = part->color();
    ui->RedSpinBox->setValue(color.red());
    ui->GreenSpinBox->setValue(color.green());
    ui->BlueSpinBox->setValue(color.blue());

    ui->checkBox->setChecked(part->Visible());

    updateColorPreview();
}

void Option_Dialog::updateColorPreview() {
    int r = ui->RedSpinBox->value();
    int g = ui->GreenSpinBox->value();
    int b = ui->BlueSpinBox->value();

    QString style = QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);
    ui->colorPreviewLabel->setStyleSheet(style);
}

void Option_Dialog::accept() {
    if (currentPart) {
        QString name = ui->lineEdit->text();
        int r = ui->RedSpinBox->value();
        int g = ui->GreenSpinBox->value();
        int b = ui->BlueSpinBox->value();
        bool Visible = ui->checkBox->isChecked();

        currentPart->setName(name);
        currentPart->setColor(QColor(r, g, b));
        currentPart->setVisible(Visible);
    }

    QDialog::accept();
}

void Option_Dialog::getModelPartData(QString &name, int &r, int &g, int &b, bool &Visible) const {
    name = ui->lineEdit->text();
    r = ui->RedSpinBox->value();
    g = ui->GreenSpinBox->value();
    b = ui->BlueSpinBox->value();
    Visible = ui->checkBox->isChecked();
}

