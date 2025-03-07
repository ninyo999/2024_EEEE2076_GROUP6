#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QModelIndex>
#include "ModelPart.h"
#include "ui_mainwindow.h"
#include "Option_Dialog.h"
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkMapper.h>
#include <vtkPlaneSource.h>  // Include the vtkPlaneSource header
#include <vtkLight.h>  // Include the vtkLight header





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect button signal to slot
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleButton);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);

    // Connect status bar signal to status bar slot
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onLightIntensityChanged);

    // Create and initialize the ModelPartList
    this->partList = new ModelPartList("PartsList");

    // Link it to the TreeView in the GUI
    ui->treeView->setModel(this->partList);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    /* This needs adding to MainWindow constructor */
    /* Link a render window with the Qt widget */
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    /* Add a renderer */
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    // Set the background color to white (RGB: 1, 1, 1)
    renderer->SetBackground(1.0, 1.0, 1.0);  // White background

   
    
    
    // Create a floor
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

    /* Will just copy and paste cylinder example from before */
    // This creates a polygonal cylinder model with eight circumferential facets
    // (i.e, in practice an octagonal prism).
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    // The mapper is responsible for pushing the geometry into the graphics
    // library. It may also do color mapping, if scalars or other attributes are
    // defined.
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // The actor is a grouping mechanism: besides the geometry (mapper), it
    // also has a property, transformation matrix, and/or texture map.
    // Here we set its color and rotate it around the X and Y axes.
    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1., 0., 0.35);
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(-45.0);

    renderer->AddActor(cylinderActor);

    /* Reset Camera (probably needs to go in its own function that is called whenever model is changed) */
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);

    renderer->ResetCameraClippingRange();

    // Create a root item
    ModelPart* rootItem = this->partList->getRootItem();

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

void MainWindow::handleButton() {
    QMessageBox msgBox;
    msgBox.setText("Add button was clicked");
    msgBox.exec();

    // Emit signal to update status bar
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
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
        }
        else {
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
void MainWindow::on_pushButton_2_clicked()
{
    openOptionDialog();
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

    Option_Dialog dialog(this);
    dialog.setModelPart(selectedPart);  // Pass the data into dialog

    if (dialog.exec() == QDialog::Accepted) {
        // Get the updated data from the dialog
        QString name;
        int r, g, b;
        bool visible;
        dialog.getModelPartData(name, r, g, b, visible);

        // Update the tree item
        selectedPart->setName(name);
        selectedPart->setColor(QColor(r, g, b));
        selectedPart->setVisible(visible);
        //selectedPart->set(1, visible ? "true" : "false"); // Store visibility text

        // Refresh the UI
        //QModelIndex parentIndex = index.parent();
        //ui->treeView->model()->dataChanged(index, index);
        //ui->treeView->model()->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});
        //ui->treeView->update();

        // **FORCE UI REFRESH**
        //QAbstractItemModel *model = ui->treeView->model();
        //model->dataChanged(index, index, {Qt::DisplayRole, Qt::ForegroundRole});

        //ui->treeView->model()->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});

        qDebug() << "Updating tree view for item:" << name << "Color:" << r << g << b;

        QAbstractItemModel *model = ui->treeView->model();
        emit model->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});


        ui->treeView->update();
        ui->treeView->viewport()->update();


        //emit statusUpdateMessage("Item Options Selected", 0);

        //QModelIndex index = ui->treeView->currentIndex();
        //emit ui->treeView->model()->dataChanged(index, index);

    }

}

void MainWindow::openOptionDialog() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) {
        emit statusUpdateMessage("No item selected.", 0);
        return;
    }

    // Get the selected item
    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (!selectedPart) return;

    // Open dialog and pass selected item
    Option_Dialog dialog(this);
    dialog.setModelPart(selectedPart);

    if (dialog.exec() == QDialog::Accepted) {  // If user clicks OK
        emit statusUpdateMessage("Updated: " + selectedPart->data(0).toString(), 0);

        // Notify TreeView model to refresh
        ui->treeView->model()->dataChanged(index, index);
    }
}



void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    if (index.isValid()) {
        ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

        // Retrieve the actor from the selected part and add it to the renderer
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor) {
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

//void MainWindow::openFile() {
// Open a file dialog to select a file
// QString fileName = QFileDialog::getOpenFileName(
//   this,
//   tr("Open File"),
//   QDir::homePath(),
//  tr("STL Files (*.stl);;Text Files (*.txt);;All Files (*)")
//   );

/// If a file was selected, update the selected tree item name
//  if (!fileName.isEmpty()) {
//    QModelIndex index = ui->treeView->currentIndex();
//    if (!index.isValid()) return;

//    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());
//      if (!selectedPart) return;

//     QString newName = QFileInfo(fileName).fileName();  // Extract filename only
//     selectedPart->setName(newName);

// Notify the model that data has changed
//    ui->treeView->model()->dataChanged(index, index, {Qt::DisplayRole});
//   emit statusUpdateMessage("Updated item name to " + newName, 0);
//  }
//}
