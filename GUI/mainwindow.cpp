#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ModelPart.h"
#include "ui_mainwindow.h"
#include "optiondialog.h" 
#include "backgrounddialog.h"
#include "VRRenderThread.h"
#include <functional>  

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
#include <vtkPlaneSource.h>
#include <vtkLight.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkTexturedActor2D.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkTexture.h>
#include <vtkActor2D.h>
#include <vtkSTLReader.h>
#include <vtkClipDataSet.h>
#include <vtkShrinkFilter.h>
#include <vtkPlane.h>
#include <vtkGeometryFilter.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: ;");
	vrThread = nullptr;
    // Connect signals to slots
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onLightIntensityChanged);
    connect(ui->toggleTreeViewButton, &QPushButton::clicked, this, &MainWindow::toggleTreeView);
    connect(ui->backgroundButton, &QPushButton::clicked, this, &MainWindow::onBackgroundButtonClicked);
    connect(ui->clipFilterCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onClipFilterCheckboxChanged);
    connect(ui->shrinkFilterCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onShrinkFilterCheckboxChanged);
	connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::startVR);
    // Connect status bar signal
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
    renderer->SetBackground(1.0, 1.0, 1.0);

    // Create a floor (grid)
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetCenter(0.05, .0, 0.0);
    planeSource->SetNormal(0.0, 0.0, 1.0);
    planeSource->SetResolution(30, 30);

    vtkNew<vtkPolyDataMapper> planeMapper;
    planeMapper->SetInputConnection(planeSource->GetOutputPort());

    gridActor = vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(planeMapper);
    gridActor->GetProperty()->SetRepresentationToWireframe();
    gridActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    gridActor->GetProperty()->SetLineWidth(1.0);
    gridActor->SetScale(3000.0, 3000.0, 3000.0);

    // Add the floor to the renderer
    renderer->AddActor(gridActor);

    // Create a light
    light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToSceneLight();
    light->SetPosition(0, 0, 1);
    light->SetFocalPoint(0, 0, 0);
    light->SetDiffuseColor(1.0, 1.0, 1.0);
    light->SetAmbientColor(1.0, 1.0, 1.0);
    light->SetSpecularColor(1.0, 1.0, 1.0);
    light->SetIntensity(0.5);
    renderer->AddLight(light);

    /* Create an object and add to renderer */
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
    ModelPart* rootItem = this->partList->getRootItem();

    // Add 1 top-level item named "Module"
    QString name = "Module";
    QString visible("true");

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

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessage(QString("The selected item is: ") + text, 0);

        // Update filter checkboxes based on current part's state
        ui->clipFilterCheckBox->blockSignals(true);
        ui->clipFilterCheckBox->setChecked(selectedPart->isClipFilterEnabled());
        ui->clipFilterCheckBox->blockSignals(false);

        ui->shrinkFilterCheckBox->blockSignals(true);
        ui->shrinkFilterCheckBox->setChecked(selectedPart->isShrinkFilterEnabled());
        ui->shrinkFilterCheckBox->blockSignals(false);
    }
}

void MainWindow::on_actionOpen_File_triggered() {
    emit statusUpdateMessage(QString("Open File action triggered"), 0);
}

void MainWindow::openFile() {
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Open STL Files"),
        QDir::homePath(),
        tr("STL Files (*.stl);;All Files (*)")
    );

    if (!fileNames.isEmpty()) {
        QModelIndex index = ui->treeView->currentIndex();
        ModelPart* selectedPart = nullptr;

        if (!index.isValid()) {
            selectedPart = partList->getRootItem();
        }
        else {
            selectedPart = static_cast<ModelPart*>(index.internalPointer());
        }

        int totalFiles = fileNames.size();
        int currentFile = 0;

        for (const QString& fileName : fileNames) {
            currentFile++;

            emit statusUpdateMessage(QString("Loading file %1 of %2: %3")
                .arg(currentFile)
                .arg(totalFiles)
                .arg(fileName), 0);

            QList<QVariant> data = { QFileInfo(fileName).fileName(), "true" };
            QModelIndex newIndex = partList->appendChild(index, data);

            ModelPart* newPart = static_cast<ModelPart*>(newIndex.internalPointer());
            newPart->loadSTL(fileName);

            renderer->AddActor(newPart->getActor());
        }

        updateRender();
        emit statusUpdateMessage(QString("Loaded %1 files successfully").arg(totalFiles), 3000);
    }
}

void MainWindow::showTreeContextMenu(const QPoint& pos) {
    QMenu contextMenu(this);

    QAction* itemOptions = new QAction("Item Options", this);
    connect(itemOptions, &QAction::triggered, this, &MainWindow::on_actionItemOptions_triggered);

    contextMenu.addAction(itemOptions);
    contextMenu.exec(ui->treeView->mapToGlobal(pos));
}

void MainWindow::on_actionItemOptions_triggered() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
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

        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor) {
            actor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        }

        QAbstractItemModel* model = ui->treeView->model();
        emit model->dataChanged(index, index, { Qt::DisplayRole, Qt::BackgroundRole });

        updateRender();
    }
}

void MainWindow::onLightIntensityChanged(int value) {
    double intensity = static_cast<double>(value) / 90.0;
    light->SetIntensity(intensity);
    renderer->Render();
}

void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    renderer->AddActor(gridActor);
    updateRenderFromTree(partList->index(0, 0, QModelIndex()));
    renderer->Render();
}

void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    if (index.isValid()) {
        ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor && selectedPart->visible()) {
            renderer->AddActor(actor);
        }

        if (!partList->hasChildren(index) || (index.flags() & Qt::ItemNeverHasChildren)) {
            return;
        }

        int rows = partList->rowCount(index);
        for (int i = 0; i < rows; i++) {
            updateRenderFromTree(partList->index(i, 0, index));
        }
    }
}

void MainWindow::toggleTreeView() {
    bool isVisible = ui->treeView->isVisible();
    ui->treeView->setVisible(!isVisible);

    if (isVisible) {
        ui->toggleTreeViewButton->setText("Show Tree View");
    }
    else {
        ui->toggleTreeViewButton->setText("Hide Tree View");
    }
}

void MainWindow::onDeleteRequested() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPartList* model = static_cast<ModelPartList*>(ui->treeView->model());
    model->removeRow(index.row(), index.parent());
    updateRender();
}

void MainWindow::onBackgroundButtonClicked() {
    BackgroundDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getSelectedType() == BackgroundDialog::SolidColor) {
            setSolidColorBackground(dialog.getSelectedColor());
        }
        else {
            setCustomImageBackground(dialog.getImageFilePath());
        }
    }
}

void MainWindow::setCustomImageBackground(const QString& imagePath) {
    if (backgroundActor) {
        renderer->RemoveActor2D(backgroundActor);
        backgroundActor = nullptr;
    }

    if (imagePath.isEmpty()) {
        setSolidColorBackground(Qt::gray);
        return;
    }

    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(imagePath.toStdString().c_str());

    if (imageReader) {
        imageReader->SetFileName(imagePath.toStdString().c_str());
        imageReader->Update();

        vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imageReader->GetOutputPort());

        vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
        plane->SetOrigin(0, 0, 0);
        plane->SetPoint1(1, 0, 0);
        plane->SetPoint2(0, 1, 0);

        vtkSmartPointer<vtkPolyDataMapper2D> mapper2D = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper2D->SetInputConnection(plane->GetOutputPort());

        backgroundActor = vtkSmartPointer<vtkTexturedActor2D>::New();
        backgroundActor->SetMapper(mapper2D);
        backgroundActor->SetTexture(texture);
        backgroundActor->SetPosition(0, 0);
        backgroundActor->GetProperty()->SetOpacity(1.0);

        renderer->AddActor2D(backgroundActor);
        renderer->ResetCamera();
        renderWindow->Render();
    }
    else {
        setSolidColorBackground(Qt::gray);
    }
}

void MainWindow::setSolidColorBackground(const QColor& color) {
    if (backgroundActor) {
        renderer->RemoveActor(backgroundActor);
        backgroundActor = nullptr;
    }

    renderer->SetBackground(color.redF(), color.greenF(), color.blueF());
    renderer->Render();
}

void MainWindow::onClipFilterCheckboxChanged(int state) {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
    if (!part) return;

    double origin[3] = { 0.0, 0.0, 0.0 };
    double normal[3] = { 0.0, -1.0, 0.0 };

    part->applyClipFilter(state == Qt::Checked, origin, normal);
    renderWindow->Render();
}

void MainWindow::onShrinkFilterCheckboxChanged(int state) {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
    if (!part) return;

    part->applyShrinkFilter(state == Qt::Checked, 0.8);
    renderWindow->Render();
}

void MainWindow::startVR()
{
    // don’t restart if already running
    if (vrThread && vrThread->isRunning()) return;

    // create and own it
    vrThread = new VRRenderThread(this);

    // recursively walk parts
    std::function<void(ModelPart*)> walk = [&](ModelPart* part){
        // ask for brand-new mapper+actor
        vtkSmartPointer<vtkActor> newA = part->getNewActor();
        // copy over all properties (colour, opacity, etc)
        newA->SetProperty(part->getActor()->GetProperty());
        // queue it for the VR thread
        vrThread->addActorOffline(newA);
        // children
        for(int i=0; i<part->childCount(); ++i)
            walk(part->child(i));
    };

    // start from root
    walk(partList->getRootItem());

    // and off we go…
    vrThread->start();
}