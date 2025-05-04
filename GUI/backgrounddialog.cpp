#include "backgrounddialog.h"
#include "ui_backgrounddialog.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>

/**
 * @brief Constructs the BackgroundDialog.
 *
 * Initializes UI components, connects signals to slots, and sets default background mode.
 * @param parent Pointer to the parent widget.
 */
BackgroundDialog::BackgroundDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::BackgroundDialog),
    selectedType(SolidColor),
    selectedColor(Qt::gray)
{
    ui->setupUi(this);

    // Connect UI signals to their respective slots
    connect(ui->solidColorRadio, &QRadioButton::toggled, this, &BackgroundDialog::onSolidColorSelected);
    connect(ui->garageImageRadio, &QRadioButton::toggled, this, &BackgroundDialog::onGarageImageSelected);
    connect(ui->colorButton, &QPushButton::clicked, this, &BackgroundDialog::onColorButtonClicked);
    connect(ui->fileSelectButton, &QPushButton::clicked, this, &BackgroundDialog::onFileSelectClicked);

    // Default to solid color background
    ui->solidColorRadio->setChecked(true);
    updateColorButton();
}

/**
 * @brief Destructor for BackgroundDialog.
 */
BackgroundDialog::~BackgroundDialog()
{
    delete ui;
}

/**
 * @brief Returns the selected background type (solid color or image).
 * @return The selected BackgroundType enum value.
 */
BackgroundDialog::BackgroundType BackgroundDialog::getSelectedType() const
{
    return selectedType;
}

/**
 * @brief Returns the selected background color.
 * @return The currently selected QColor.
 */
QColor BackgroundDialog::getSelectedColor() const
{
    return selectedColor;
}

/**
 * @brief Slot triggered when solid color radio button is selected.
 */
void BackgroundDialog::onSolidColorSelected()
{
    selectedType = SolidColor;
    ui->colorButton->setEnabled(true);
}

/**
 * @brief Slot triggered when garage image radio button is selected.
 */
void BackgroundDialog::onGarageImageSelected()
{
    selectedType = GarageImage;
    ui->colorButton->setEnabled(false);
}

/**
 * @brief Slot triggered when the color button is clicked.
 * Opens a QColorDialog to select a new background color.
 */
void BackgroundDialog::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(selectedColor, this, "Select Background Color");
    if (newColor.isValid()) {
        selectedColor = newColor;
        updateColorButton();
    }
}

/**
 * @brief Updates the color button appearance to match selected color.
 */
void BackgroundDialog::updateColorButton()
{
    QString style = QString("background-color: %1; border: 1px solid black;")
        .arg(selectedColor.name());
    ui->colorButton->setStyleSheet(style);
}

/**
 * @brief Slot triggered when the file selection button is clicked.
 * Opens a file dialog to choose an image file for the background.
 */
void BackgroundDialog::onFileSelectClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Background Image"), "",
        tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    if (!fileName.isEmpty()) {
        imageFilePath = fileName;
        ui->filePathLabel->setText(QFileInfo(fileName).fileName());
    }
}

/**
 * @brief Returns the file path of the selected background image.
 * @return File path as QString.
 */
QString BackgroundDialog::getImageFilePath() const
{
    return imageFilePath;
}
