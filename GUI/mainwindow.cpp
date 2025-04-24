#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ModelPart.h"
#include "ui_mainwindow.h"
#include "optiondialog.h" 
#include "backgrounddialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkMapper.h>
#include <vtkPlaneSource.h>  // For floor (grid)
#include <vtkLight.h>        // For light control
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkTexturedActor2D.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkActor2D.h>
#include <vtkSTLReader.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: ;");

    // Connect button signal to slot

    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onLightIntensityChanged);  // Light intensity slider
	connect(ui->toggleTreeViewButton, &QPushButton::clicked, this, &MainWindow::toggleTreeView);
	connect(ui->backgroundButton, &QPushButton::clicked, this, &MainWindow::onBackgroundButtonClicked);
	connect(ui->actionSave_File, &QAction::triggered, this, &MainWindow::onSaveFile);
    // Connect status bar signal to status bar slot
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);

    // Create and initialize the ModelPartList
    this->partList = new ModelPartList("PartsList");

    // Link it to the TreeView in the GUI
    ui->treeView->setModel(this->partList);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Link a render window with the Qt widget */
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    /* Add a renderer */
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    // Set the background color to white (RGB: 1, 1, 1)
    renderer->SetBackground(1.0, 1.0, 1.0);  // White background

    // Create a floor (grid)
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetCenter(0.05, .0, 0.0);  // Center of the plane
    planeSource->SetNormal(0.0, 0.0, 1.0);  // Normal pointing upwards (Z-axis)
    planeSource->SetResolution(30, 30);     // Resolution of the plane

    vtkNew<vtkPolyDataMapper> planeMapper;
    planeMapper->SetInputConnection(planeSource->GetOutputPort());

    gridActor = vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(planeMapper);

    // Set the floor to wireframe representation
    gridActor->GetProperty()->SetRepresentationToWireframe();  // Grid-like appearance
    gridActor->GetProperty()->SetColor(0.5, 0.5, 0.5);        // Gray color
    gridActor->GetProperty()->SetLineWidth(1.0);              // Thickness of the grid lines
    gridActor->SetScale(3000.0, 3000.0, 3000.0);

    // Add the floor to the renderer
    renderer->AddActor(gridActor);

    // Create a light
    light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToSceneLight();  // Positional light
    light->SetPosition(0, 0, 1);      // Position the light above the floor
    light->SetFocalPoint(0, 0, 0);     // Focus the light on the floor
    light->SetDiffuseColor(1.0, 1.0, 1.0);  // White light
    light->SetAmbientColor(1.0, 1.0, 1.0);  // Ambient light
    light->SetSpecularColor(1.0, 1.0, 1.0); // Specular light
    light->SetIntensity(0.5);  // Increase intensity
    renderer->AddLight(light);

    /* Create an object and add to renderer (this will change later to display a CAD model) */
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1., 0., 0.35);
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(-45.0);

    renderer->AddActor(cylinderActor);

    /* Reset Camera */
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

    // Create a root item
    ModelPart *rootItem = this->partList->getRootItem();

    // Add 1 top-level item named "Module"
    QString name = "Module";  // Name of the top-level item
    QString visible("true");  // Visibility of the item

    // Create the top-level item
    ModelPart* moduleItem = new ModelPart({ name, visible });
    rootItem->appendChild(moduleItem);

    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleTreeClicked() {
    QModelIndex index = ui->treeView->currentIndex();

    if (!index.isValid()) return;

    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessage(QString("The selected item is: ") + text, 0);
    }
}

void MainWindow::on_actionOpen_File_triggered() {
    emit statusUpdateMessage(QString("Open File action triggered"), 0);
}

void MainWindow::openFile() {
    // Open a file dialog to select multiple STL files
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Open STL Files"),
        QDir::homePath(),
        tr("STL Files (*.stl);;All Files (*)")
    );

    // If files were selected, process each one
    if (!fileNames.isEmpty()) {
        QModelIndex index = ui->treeView->currentIndex();
        ModelPart* selectedPart = nullptr;

        // If no item is selected, add the new parts as children of the root item
        if (!index.isValid()) {
            selectedPart = partList->getRootItem();
        } else {
            selectedPart = static_cast<ModelPart*>(index.internalPointer());
        }

        int totalFiles = fileNames.size();
        int currentFile = 0;

        // Loop through each selected file
        for (const QString& fileName : fileNames) {
            currentFile++;

            // Update the status bar
            emit statusUpdateMessage(QString("Loading file %1 of %2: %3")
                .arg(currentFile)
                .arg(totalFiles)
                .arg(fileName), 0);

            // Create a new ModelPart for the STL file
            QList<QVariant> data = { QFileInfo(fileName).fileName(), "true" };
            QModelIndex newIndex = partList->appendChild(index, data);

            // Load the STL file into the new ModelPart
            ModelPart* newPart = static_cast<ModelPart*>(newIndex.internalPointer());
            newPart->loadSTL(fileName);

            // Add the actor to the renderer
            renderer->AddActor(newPart->getActor());
        }

        // Update the renderer to reflect the changes
        updateRender();

        // Notify the user that loading is complete
        emit statusUpdateMessage(QString("Loaded %1 files successfully").arg(totalFiles), 3000);
    }
}

void MainWindow::showTreeContextMenu(const QPoint &pos) {
    QMenu contextMenu(this);

    QAction *itemOptions = new QAction("Item Options", this);
    connect(itemOptions, &QAction::triggered, this, &MainWindow::on_actionItemOptions_triggered);

    contextMenu.addAction(itemOptions);
    contextMenu.exec(ui->treeView->mapToGlobal(pos));
}

void MainWindow::on_actionItemOptions_triggered() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (!selectedPart) return;

    OptionDialog dialog(this);
    dialog.setModelPart(selectedPart);
    connect(&dialog, &OptionDialog::deleteRequested, this, &MainWindow::onDeleteRequested);

    if (dialog.exec() == QDialog::Accepted) {
        QString name;
        QColor color;
        bool visible;
        dialog.getModelPartData(name, color, visible);

        selectedPart->setName(name);
        selectedPart->setColor(color);
        selectedPart->setVisible(visible);

        // Update the VTK actor color
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor) {
            actor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        }

        QAbstractItemModel *model = ui->treeView->model();
        emit model->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});

        updateRender();
    }
}

void MainWindow::onLightIntensityChanged(int value) {
    // Map the slider value (0-100) to a light intensity range (e.g., 0.0 to 2.0)
    double intensity = static_cast<double>(value) / 90.0;  // Adjust the scaling factor as needed

    // Update the light intensity
    light->SetIntensity(intensity);

    // Refresh the renderer to reflect the changes
    renderer->Render();
}

void MainWindow::updateRender() {
    // Remove all actors except the floor
    renderer->RemoveAllViewProps();

    // Re-add the floor actor
    renderer->AddActor(gridActor);

    // Update the renderer with actors from the tree
    updateRenderFromTree(partList->index(0, 0, QModelIndex()));

    // Force an immediate update of the renderer
    renderer->Render();
}

void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    if (index.isValid()) {
        ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

        // Retrieve the actor from the selected part and add it to the renderer
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor && selectedPart->visible()) {
            renderer->AddActor(actor);
        }

        // Check if this part has any children
        if (!partList->hasChildren(index) || (index.flags() & Qt::ItemNeverHasChildren)) {
            return;
        }

        // Loop through children and add their actors
        int rows = partList->rowCount(index);
        for (int i = 0; i < rows; i++) {
            updateRenderFromTree(partList->index(i, 0, index));
        }
    }
}

void MainWindow::toggleTreeView() {
    bool isVisible = ui->treeView->isVisible();
    ui->treeView->setVisible(!isVisible);  // Toggle visibility

    // Change button text based on visibility
    if (isVisible) {
        ui->toggleTreeViewButton->setText("Show Tree View");
    } else {
        ui->toggleTreeViewButton->setText("Hide Tree View");
    }
}

void MainWindow::onDeleteRequested() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPartList *model = static_cast<ModelPartList*>(ui->treeView->model());
    model->removeRow(index.row(), index.parent());  // Remove the row from the model

    // Refresh the VTK renderer
    updateRender();
}

void MainWindow::onBackgroundButtonClicked()
{
    BackgroundDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getSelectedType() == BackgroundDialog::SolidColor) {
            setSolidColorBackground(dialog.getSelectedColor());
        } else {
            setCustomImageBackground(dialog.getImageFilePath());
        }
    }
}
void MainWindow::setCustomImageBackground(const QString& imagePath)
{
    // Remove existing background
    if (backgroundActor) {
        renderer->RemoveActor2D(backgroundActor);
        backgroundActor = nullptr;
    }

    if (imagePath.isEmpty()) {
        setSolidColorBackground(Qt::gray);
        return;
    }

    // Load the image
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(imagePath.toStdString().c_str());
    
    if (imageReader) {
        imageReader->SetFileName(imagePath.toStdString().c_str());
        imageReader->Update();

        // Create texture
        vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imageReader->GetOutputPort());

        // Create plane
        vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
        plane->SetOrigin(0, 0, 0);  // Simplified coordinates for 2D
        plane->SetPoint1(1, 0, 0);
        plane->SetPoint2(0, 1, 0);

        // Create 2D mapper
        vtkSmartPointer<vtkPolyDataMapper2D> mapper2D = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper2D->SetInputConnection(plane->GetOutputPort());

        // Create 2D actor (single consistent creation)
        backgroundActor = vtkSmartPointer<vtkTexturedActor2D>::New();
        backgroundActor->SetMapper(mapper2D);
        backgroundActor->SetTexture(texture);

        // Position the actor (2D coordinates only)
        backgroundActor->SetPosition(0, 0);
        
        // Set display properties
        backgroundActor->GetProperty()->SetOpacity(1.0);
        
        // Add to renderer
        renderer->AddActor2D(backgroundActor);
        renderer->ResetCamera();
        renderWindow->Render();
    } else {
        setSolidColorBackground(Qt::gray);
    }
}


void MainWindow::setSolidColorBackground(const QColor& color)
{
    // Remove any existing background actor
    if (backgroundActor) {
        renderer->RemoveActor(backgroundActor);
        backgroundActor = nullptr;
    }
    
    // Set the background color
    renderer->SetBackground(color.redF(), color.greenF(), color.blueF());
    renderer->Render();
}

void MainWindow::setGarageImageBackground()
{
    // First remove any existing background
    if (backgroundActor) {
        renderer->RemoveActor(backgroundActor);
        backgroundActor = nullptr;
    }
    
    // Load the garage image 
    QString imagePath = "C:/Users/Creative/2024_EEEE2076_GROUP6/GUI/icons/garageBackground.jpg"; 
    
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(imagePath.toStdString().c_str());
    
    if (imageReader) {
        imageReader->SetFileName(imagePath.toStdString().c_str());
        imageReader->Update();
        
        // Create texture
        vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imageReader->GetOutputPort());
        
        // Create a plane for the background
        vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
        plane->SetOrigin(0, 0, 0);
        plane->SetPoint1(1, 0, 0);
        plane->SetPoint2(0, 1, 0);
        
        // Create actor for the background
        backgroundActor = vtkSmartPointer<vtkTexturedActor2D>::New();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputConnection(plane->GetOutputPort());
        backgroundActor->SetMapper(mapper);
        backgroundActor->SetTexture(texture);
        
        // Add to renderer (behind everything else)
        renderer->AddActor(backgroundActor);
        backgroundActor->GetProperty()->SetDisplayLocationToBackground();
        
        renderer->Render();
    } else {
        QMessageBox::warning(this, "Error", "Could not load garage image. Please check the file path.");
    }
}


void MainWindow::onSaveFile()
{
    // Get save file location
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Model Settings"), "",
        tr("JSON Files (*.json);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    // Prepare JSON structure
    QJsonObject rootObject;
    
    // 1. Save background settings
    QJsonObject background;
    if (backgroundActor) {
        background["type"] = "image";
        //background["path"] = currentBackgroundPath; // Store your member variable holding the path
    } else {
        background["type"] = "color";
        double* bgColor = renderer->GetBackground();
        background["color"] = QString("%1,%2,%3")
            .arg(bgColor[0])
            .arg(bgColor[1])
            .arg(bgColor[2]);
    }
    rootObject["background"] = background;

    // 2. Save model parts
    QJsonArray partsArray;
    QList<ModelPart*> parts = partList->getAllParts();
    
    for (ModelPart* part : parts) {
        QJsonObject partObj;
        partObj["name"] = part->data(0).toString();
        
        QColor color = part->color();
        partObj["color"] = QString("%1,%2,%3")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());
            
        partObj["visible"] = part->visible();
        partObj["filePath"] = part->getFilePath();
        
        partsArray.append(partObj);
    }
    rootObject["parts"] = partsArray;

    // 3. Save to file
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing");
        return;
    }

    saveFile.write(QJsonDocument(rootObject).toJson());
    saveFile.close();
    
    statusUpdateMessage("Settings saved successfully", 3000);
}