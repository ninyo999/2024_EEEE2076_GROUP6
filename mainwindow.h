#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include "ModelPartList.h"
#include "ModelPart.h"
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleButton();
    void handleTreeClicked();
    void on_actionOpen_File_triggered();
    void openFile();
    void openOptionDialog();
    void showTreeContextMenu(const QPoint &pos);
    void on_actionItemOptions_triggered();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);


signals:
    void statusUpdateMessage(const QString &message, int timeout);

private slots:
    void on_pushButton_2_clicked();
    void onLightIntensityChanged(int value);

private:
    Ui::MainWindow *ui;
    ModelPartList *partList;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    // Floor actor
    
    vtkSmartPointer<vtkActor> gridActor;

    // Light
    vtkSmartPointer<vtkLight> light;


};




#endif // MAINWINDOW_H
