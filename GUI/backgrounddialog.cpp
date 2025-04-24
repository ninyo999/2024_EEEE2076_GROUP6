#include "backgrounddialog.h"
#include "ui_backgrounddialog.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>

BackgroundDialog::BackgroundDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundDialog),
    selectedType(SolidColor),
    selectedColor(Qt::gray)
{
    ui->setupUi(this);
    
    // Connect signals
    connect(ui->solidColorRadio, &QRadioButton::toggled, this, &BackgroundDialog::onSolidColorSelected);
    connect(ui->garageImageRadio, &QRadioButton::toggled, this, &BackgroundDialog::onGarageImageSelected);
    connect(ui->colorButton, &QPushButton::clicked, this, &BackgroundDialog::onColorButtonClicked);
    connect(ui->fileSelectButton, &QPushButton::clicked, this, &BackgroundDialog::onFileSelectClicked);
    // Set initial state
    ui->solidColorRadio->setChecked(true);
    updateColorButton();
}

BackgroundDialog::~BackgroundDialog()
{
    delete ui;
}

BackgroundDialog::BackgroundType BackgroundDialog::getSelectedType() const
{
    return selectedType;
}

QColor BackgroundDialog::getSelectedColor() const
{
    return selectedColor;
}

void BackgroundDialog::onSolidColorSelected()
{
    selectedType = SolidColor;
    ui->colorButton->setEnabled(true);
}

void BackgroundDialog::onGarageImageSelected()
{
    selectedType = GarageImage;
    ui->colorButton->setEnabled(false);
}

void BackgroundDialog::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(selectedColor, this, "Select Background Color");
    if (newColor.isValid()) {
        selectedColor = newColor;
        updateColorButton();
    }
}

void BackgroundDialog::updateColorButton()
{
    QString style = QString("background-color: %1; border: 1px solid black;").arg(selectedColor.name());
    ui->colorButton->setStyleSheet(style);
}

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

QString BackgroundDialog::getImageFilePath() const
{
    return imageFilePath;
}