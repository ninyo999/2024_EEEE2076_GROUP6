#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
    , currentPart(nullptr)
{
    ui->setupUi(this);
	


    connect(ui->spinBoxR, QOverload<int>::of(&QSpinBox::valueChanged), this, &OptionDialog::updateColorPreview);
    connect(ui->spinBoxG, QOverload<int>::of(&QSpinBox::valueChanged), this, &OptionDialog::updateColorPreview);
    connect(ui->spinBoxB, QOverload<int>::of(&QSpinBox::valueChanged), this, &OptionDialog::updateColorPreview);
}

OptionDialog::~OptionDialog() {
    delete ui;
}

void OptionDialog::setModelPart(ModelPart *part) {
    if (!part) return;

    currentPart = part;
    ui->lineEdit->setText(part->data(0).toString());

    QColor color = part->color();
    ui->spinBoxR->setValue(color.red());
    ui->spinBoxG->setValue(color.green());
    ui->spinBoxB->setValue(color.blue());

    ui->checkBox->setChecked(part->visible());

    updateColorPreview();
}

void OptionDialog::updateColorPreview() {
    int r = ui->spinBoxR->value();
    int g = ui->spinBoxG->value();
    int b = ui->spinBoxB->value();

    QString style = QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);
    ui->colorPreviewLabel->setStyleSheet(style);
}

void OptionDialog::accept() {
    if (currentPart) {
        QString name = ui->lineEdit->text();
        int r = ui->spinBoxR->value();
        int g = ui->spinBoxG->value();
        int b = ui->spinBoxB->value();
        bool visible = ui->checkBox->isChecked();

        currentPart->setName(name);
        currentPart->setColor(QColor(r, g, b));
        currentPart->setVisible(visible);
    }

    QDialog::accept();
}

void OptionDialog::getModelPartData(QString &name, int &r, int &g, int &b, bool &visible) const {
    name = ui->lineEdit->text();
    r = ui->spinBoxR->value();
    g = ui->spinBoxG->value();
    b = ui->spinBoxB->value();
    visible = ui->checkBox->isChecked();
}