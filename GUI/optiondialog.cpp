#include "optiondialog.h"
#include "ui_optiondialog.h"
#include <QColorDialog>

/**
 * @brief Constructs the OptionDialog and connects UI signals.
 * @param parent The parent widget.
 */
OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
    , currentPart(nullptr)
{
    ui->setupUi(this);

    connect(ui->colorButton, &QPushButton::clicked, this, &OptionDialog::onColorButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &OptionDialog::onDeleteButtonClicked);
}

/**
 * @brief Destructor for OptionDialog.
 */
OptionDialog::~OptionDialog() {
    delete ui;
}

/**
 * @brief Sets the model part to edit in the dialog and populates fields.
 * @param part Pointer to the ModelPart to configure.
 */
void OptionDialog::setModelPart(ModelPart* part) {
    if (!part) return;

    currentPart = part;
    ui->lineEdit->setText(part->data(0).toString());

    currentColor = part->color();
    updateColorButton();

    ui->checkBox->setChecked(part->visible());
}

/**
 * @brief Accepts and applies changes to the selected ModelPart.
 */
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

/**
 * @brief Retrieves the model part data as entered in the dialog.
 * @param name Output name.
 * @param color Output color.
 * @param visible Output visibility flag.
 */
void OptionDialog::getModelPartData(QString& name, QColor& color, bool& visible) const {
    name = ui->lineEdit->text();
    color = currentColor;
    visible = ui->checkBox->isChecked();
}

/**
 * @brief Handles the delete button click event.
 * Emits deleteRequested and closes the dialog.
 */
void OptionDialog::onDeleteButtonClicked() {
    emit deleteRequested();
    this->reject();
}

/**
 * @brief Updates the color button's background to match currentColor.
 */
void OptionDialog::updateColorButton() {
    QString style = QString("background-color: %1; border: 1px solid black;").arg(currentColor.name());
    ui->colorButton->setStyleSheet(style);
}

/**
 * @brief Opens a color dialog and updates the selected color.
 */
void OptionDialog::onColorButtonClicked() {
    QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color");
    if (newColor.isValid()) {
        currentColor = newColor;
        updateColorButton();
    }
}
