// optiondialog.h
#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QColor>
#include "ModelPart.h"

/**
 * @file
 * This file contains the declarations for the OptionDialog class,
 * which provides UI to view and modify properties of a ModelPart.
 */

namespace Ui {
    class OptionDialog;
}

/**
 * @class OptionDialog
 * @brief A dialog to edit properties of a 3D model part.
 *
 * This class allows users to edit the name, color, and visibility
 * of a ModelPart. It also provides options to delete the part.
 */
class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the OptionDialog.
     * @param parent Pointer to the parent widget.
     */
    explicit OptionDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor for OptionDialog.
     */
    ~OptionDialog();

    /**
     * @brief Retrieves data entered in the dialog.
     * @param name The updated name of the model part.
     * @param color The selected color.
     * @param visible Visibility status.
     */
    void getModelPartData(QString& name, QColor& color, bool& visible) const;

    /**
     * @brief Sets the ModelPart whose data will be shown in the dialog.
     * @param part Pointer to the ModelPart.
     */
    void setModelPart(ModelPart* part);

    /**
     * @brief Handles the accept/OK button press.
     */
    void accept() override;

    /**
     * @brief Updates the color button appearance.
     */
    void updateColorButton();

signals:
    /**
     * @brief Emitted when the user requests deletion of the part.
     */
    void deleteRequested();

private slots:
    /**
     * @brief Handles color button click to open a color picker.
     */
    void onColorButtonClicked();

    /**
     * @brief Handles delete button click.
     */
    void onDeleteButtonClicked();

private:
    Ui::OptionDialog* ui;         /**< Pointer to UI elements */
    ModelPart* currentPart;       /**< Currently selected model part */
    QColor currentColor;          /**< Selected color */
};

#endif // OPTIONDIALOG_H
