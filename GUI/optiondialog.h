#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
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
    void setModelPartData(const QString &name, int r, int g, int b, bool visible);
    ModelPart* getModelPart();

    void getModelPartData(QString &name, int &r, int &g, int &b, bool &visible) const;
    void setModelPart(ModelPart *part);
    void accept() override;

signals:
    void deleteRequested();

private slots:
    void updateColorPreview();
	void onDeleteButtonClicked();  // Slot for delete button click
private:
    Ui::OptionDialog *ui;
    ModelPart *currentPart;
};

#endif // OPTIONDIALOG_H