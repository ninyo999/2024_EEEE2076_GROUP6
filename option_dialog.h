#ifndef OPTION_DIALOG_H
#define OPTION_DIALOG_H

#include <QDialog>
#include "ModelPart.h"

namespace Ui {
class Option_Dialog;
}

class Option_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Option_Dialog(QWidget *parent = nullptr);
    ~Option_Dialog();

    // Functions to set/get data from the dialog
    void setModelPartData(const QString &name, int r, int g, int b, bool Visible);
    ModelPart* getModelPart();

    void getModelPartData(QString &name, int &r, int &g, int &b, bool &Visible) const;
    void setModelPart(ModelPart *part);
    void accept() override;

private slots:
    void updateColorPreview();

private:
    Ui::Option_Dialog *ui;
    ModelPart *currentPart;
};

#endif // OPTION_DIALOG_H
