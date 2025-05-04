// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QFile>
#include <QFileDialog>
#include <QColor>
#include "skyboxutils.h"
#include "ModelPartList.h"
#include "ModelPart.h"
#include "VRRenderThread.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkActor.h>  
#include <vtkLight.h>  
#include <QTimer>
#include "backgrounddialog.h"
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkTexturedActor2D.h>
#include <vtkClipDataSet.h>
#include <vtkShrinkFilter.h>
#include <vtkPlane.h>
#include <vtkGeometryFilter.h>
#include <vtkSkybox.h> 

/**
 * @file
 * This file contains the declaration of the MainWindow class used to manage the GUI
 * and rendering functionalities of the 3D model viewer application.
 */

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The main application window handling user interaction and 3D rendering.
 *
 * The MainWindow class is responsible for managing user actions, file loading,
 * rendering models using VTK, background setup, tree view updates, VR rendering,
 * and applying filters to 3D components.
 */
    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window.
     * @param parent The parent widget.
     */
    MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructor for MainWindow.
     */
    ~MainWindow();

public slots:
    /**
     * @brief Handles clicks in the component tree view.
     */
    void handleTreeClicked();

    /**
     * @brief Triggered when the "Open File" action is selected.
     */
    void on_actionOpen_File_triggered();

    /**
     * @brief Opens a file dialog to load a model.
     */
    void openFile();

    /**
     * @brief Shows context menu when right-clicking on the tree view.
     * @param pos The position of the click.
     */
    void showTreeContextMenu(const QPoint& pos);

    /**
     * @brief Triggered when "Item Options" is selected from the menu.
     */
    void on_actionItemOptions_triggered();

    /**
     * @brief Updates the 3D render window.
     */
    void updateRender();

    /**
     * @brief Toggles the visibility of the tree view.
     */
    void toggleTreeView();

    /**
     * @brief Updates rendering based on the selected item in the tree view.
     * @param index Index of the selected tree item.
     */
    void updateRenderFromTree(const QModelIndex& index);

    /**
     * @brief Updates lighting based on the slider value.
     * @param value The light intensity value.
     */
    void onLightIntensityChanged(int value);

    /**
     * @brief Starts VR rendering.
     */
    void startVR();

    /**
     * @brief Stops VR rendering.
     */
    void stopVR();

    /**
     * @brief Loads a skybox texture into the scene.
     */
    void onLoadSkyboxClicked();

signals:
    /**
     * @brief Signal to update the status bar.
     * @param message The message to display.
     * @param timeout How long the message should be shown (ms).
     */
    void statusUpdateMessage(const QString& message, int timeout);

private slots:
    /**
     * @brief Handles the deletion of selected component(s).
     */
    void onDeleteRequested();

    /**
     * @brief Opens the background selection dialog.
     */
    void onBackgroundButtonClicked();

    /**
     * @brief Sets a custom background using an image.
     * @param imagePath Path to the background image.
     */
    void setCustomImageBackground(const QString& imagePath);

    /**
     * @brief Sets a solid color as the background.
     * @param color The QColor to set as background.
     */
    void setSolidColorBackground(const QColor& color);

    /**
     * @brief Slot triggered when the clip filter checkbox changes state.
     * @param state New state of the checkbox.
     */
    void onClipFilterCheckboxChanged(int state);

    /**
     * @brief Slot triggered when the shrink filter checkbox changes state.
     * @param state New state of the checkbox.
     */
    void onShrinkFilterCheckboxChanged(int state);

    /**
     * @brief Updates the VR renderer to reflect current scene.
     */
    void updateVRRenderer();

    /**
     * @brief Updates the VR background with a solid color.
     * @param color The color to use as VR background.
     */
    void updateVRBackground(const QColor& color);

    /**
     * @brief Updates model rotation speed.
     * @param value New rotation speed value.
     */
    void onRotationSpeedChanged(int value);

    /**
     * @brief Applies rotation to models.
     */
    void rotateModels();

    /**
     * @brief Recursively rotates a model part.
     * @param part The model part to rotate.
     * @param speed Rotation speed.
     */
    void rotatePartRecursive(ModelPart* part, int speed);

private:
    Ui::MainWindow* ui;  /**< Pointer to the UI */
    ModelPartList* partList;  /**< List of model parts */

    vtkSmartPointer<vtkRenderer> renderer;  /**< VTK renderer */
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;  /**< VTK render window */

    vtkSmartPointer<vtkActor> gridActor;  /**< Actor for background grid */
    vtkSmartPointer<vtkLight> light;  /**< Light source for the scene */
    vtkSmartPointer<vtkTexturedActor2D> backgroundActor;  /**< 2D background actor */

    VRRenderThread* vrThread = nullptr;  /**< VR rendering thread */

    /**
     * @brief Sends the model part recursively to the VR renderer.
     * @param part The model part to send.
     */
    void sendPartRecursive(ModelPart* part);

    QTimer* rotationTimer = nullptr;  /**< Timer for rotation updates */
    int rotationSpeed = 0;  /**< Current model rotation speed */

    vtkSmartPointer<vtkSkybox> skybox;  /**< Skybox for the 3D scene */
};

#endif // MAINWINDOW_H
