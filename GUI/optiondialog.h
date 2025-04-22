#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QColor>
#include "ModelPart.h"

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();

    // Functions to set/get data from the dialog

    void getModelPartData(QString &name, QColor &color, bool &visible) const;
    void setModelPart(ModelPart *part);
    void accept() override;
	void updateColorButton();

signals:
    void deleteRequested();

private slots:
    void onColorButtonClicked();
	void onDeleteButtonClicked();  // Slot for delete button click
private:
    Ui::OptionDialog *ui;
    ModelPart *currentPart;
	QColor currentColor;
};

#endif // OPTIONDIALOG_H