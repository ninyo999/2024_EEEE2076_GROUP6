// backgrounddialog.h
#ifndef BACKGROUNDDIALOG_H
#define BACKGROUNDDIALOG_H

#include <QDialog>
#include <QColor>

/**
 * @file
 * This file contains the declarations for the BackgroundDialog class,
 * which allows users to select background options such as solid colors
 * or a garage image.
 */

namespace Ui {
    class BackgroundDialog;
}

/**
 * @class BackgroundDialog
 * @brief Dialog for selecting background settings.
 *
 * This class provides options for users to select between a solid color
 * or a garage image as the background. It also allows color selection
 * and file selection.
 */
class BackgroundDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the BackgroundDialog.
     * @param parent Pointer to the parent widget.
     */
    explicit BackgroundDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor for BackgroundDialog.
     */
    ~BackgroundDialog();

    /**
     * @enum BackgroundType
     * @brief Enum representing the background type.
     */
    enum BackgroundType {
        SolidColor,   /**< Use a solid color as background */
        GarageImage   /**< Use a garage image as background */
    };

    /**
     * @brief Gets the selected background type.
     * @return The selected BackgroundType.
     */
    BackgroundType getSelectedType() const;

    /**
     * @brief Gets the selected color.
     * @return The selected QColor.
     */
    QColor getSelectedColor() const;

    /**
     * @brief Gets the file path of the selected garage image.
     * @return The image file path as a QString.
     */
    QString getImageFilePath() const;

private slots:
    /**
     * @brief Slot triggered when solid color option is selected.
     */
    void onSolidColorSelected();

    /**
     * @brief Slot triggered when garage image option is selected.
     */
    void onGarageImageSelected();

    /**
     * @brief Slot triggered when color button is clicked.
     */
    void onColorButtonClicked();

    /**
     * @brief Updates the color button to reflect selected color.
     */
    void updateColorButton();

    /**
     * @brief Slot triggered when file select button is clicked.
     */
    void onFileSelectClicked();

private:
    Ui::BackgroundDialog* ui;       /**< Pointer to the UI form */
    BackgroundType selectedType;    /**< Currently selected background type */
    QColor selectedColor;           /**< Currently selected color */
    QString imageFilePath;          /**< Selected garage image file path */
};

#endif // BACKGROUNDDIALOG_H
