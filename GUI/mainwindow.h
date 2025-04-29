#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeView>
#include <QFile>
#include <QFileDialog>
#include <QColor>
#include <QMainWindow>


#include "VRRenderThread.h"
#include "ModelPartList.h"
#include "ModelPart.h"
#include <vtkSmartPointer.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>  // For gridActor
#include <vtkLight.h>  // For light

// For filters
#include <vtkClipDataSet.h>
#include <vtkShrinkFilter.h>
#include <vtkPlane.h>
#include <vtkGeometryFilter.h>

// For BackgroundDialog
#include "backgrounddialog.h"
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkTexturedActor2D.h>




QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void handleTreeClicked();
    void on_actionOpen_File_triggered();
    void openFile();
    void showTreeContextMenu(const QPoint& pos);
    void on_actionItemOptions_triggered();
    void updateRender();
    void toggleTreeView();
    void updateRenderFromTree(const QModelIndex& index);
    void onLightIntensityChanged(int value);

signals:
    void statusUpdateMessage(const QString& message, int timeout);

private slots:
    void onDeleteRequested();
    void onBackgroundButtonClicked();
    void setCustomImageBackground(const QString& imagePath);
    void setSolidColorBackground(const QColor& color);
    void onClipFilterCheckboxChanged(int state);
    void onShrinkFilterCheckboxChanged(int state);
    void onStartVRClicked();

private:
    Ui::MainWindow* ui;
    ModelPartList* partList;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    // Floor actor
    vtkSmartPointer<vtkActor> gridActor;

    // Light
    vtkSmartPointer<vtkLight> light;

    // Background
    vtkSmartPointer<vtkTexturedActor2D> backgroundActor;
	
	vtkSmartPointer<vtkRenderer> mainRenderer;
    VRRenderThread* vrThread;
};

#endif // MAINWINDOW_H