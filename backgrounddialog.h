#ifndef BACKGROUNDDIALOG_H
#define BACKGROUNDDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui {
class BackgroundDialog;
}

class BackgroundDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundDialog(QWidget *parent = nullptr);
    ~BackgroundDialog();

    enum BackgroundType {
        SolidColor,
        GarageImage
    };

    BackgroundType getSelectedType() const;
    QColor getSelectedColor() const;
	QString getImageFilePath() const;

private slots:
    void onSolidColorSelected();
    void onGarageImageSelected();
    void onColorButtonClicked();
	void updateColorButton();
	void onFileSelectClicked();

private:
    Ui::BackgroundDialog *ui;
    BackgroundType selectedType;
    QColor selectedColor;
	QString imageFilePath;
};

#endif // BACKGROUNDDIALOG_H