#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>

#include "ModelPartList.h"
#include "ModelPart.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkActor.h>  // For gridActor
#include <vtkLight.h>  // For light
//For BackgroundDialog
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
    void onLightIntensityChanged(int value);  
	void onSaveFile();

signals:
    void statusUpdateMessage(const QString &message, int timeout);

private slots:
    void onDeleteRequested();  
	void onBackgroundButtonClicked();
    void setCustomImageBackground(const QString& imagePath);

private:
    Ui::MainWindow *ui;
    ModelPartList *partList;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    // Floor actor (from your friend's code)
    vtkSmartPointer<vtkActor> gridActor;

    // Light (from your friend's code)
    vtkSmartPointer<vtkLight> light;
	//VRRenderThread *vrThread;  
	
	//For BackgroundDialog
	void setSolidColorBackground(const QColor& color);
    void setGarageImageBackground();
    vtkSmartPointer<vtkTexturedActor2D> backgroundActor;
	
};

#endif // MAINWINDOW_H