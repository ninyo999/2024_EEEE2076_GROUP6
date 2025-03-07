#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include "ModelPartList.h"
#include "ModelPart.h"
#include "VRRenderThread.h"
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkActor.h>  // For gridActor
#include <vtkLight.h>  // For light

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

    void handleTreeClicked();
    void on_actionOpen_File_triggered();
    void openFile();
    void showTreeContextMenu(const QPoint &pos);
    void on_actionItemOptions_triggered();
    void updateRender();
    void toggleTreeView();  
	void updateRenderFromTree(const QModelIndex& index);
    void toggleVR();  
    void onLightIntensityChanged(int value);  


signals:
    void statusUpdateMessage(const QString &message, int timeout);

private slots:
    void onDeleteRequested();  

    

private:
    Ui::MainWindow *ui;
    ModelPartList *partList;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    // Floor actor (from your friend's code)
    vtkSmartPointer<vtkActor> gridActor;

    // Light (from your friend's code)
    vtkSmartPointer<vtkLight> light;
	VRRenderThread *vrThread;  
};

#endif // MAINWINDOW_H