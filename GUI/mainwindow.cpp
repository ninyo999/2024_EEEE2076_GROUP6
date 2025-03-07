#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ModelPart.h"
#include "ui_mainwindow.h"
#include "optiondialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QPoint>

#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkMapper.h>





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	this->setStyleSheet("background-color: ;");
    // Connect button signal to slot
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleButton);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
	
    // Connect status bar signal to status bar slot
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);

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
    ModelPart *rootItem = this->partList->getRootItem();

   
    
        QString name = QString("TopLevel");
        QString visible("true");

        // Create child item
        ModelPart *childItem = new ModelPart({name, visible});
        rootItem->appendChild(childItem);

     
    
	connect(ui->toggleTreeViewButton, &QPushButton::clicked, this, &MainWindow::toggleTreeView);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    // Open a file dialog to select a file
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QDir::homePath(),
        tr("STL Files (*.stl);;All Files (*)")
    );

    // If a file was selected, add it to the tree view
    if (!fileName.isEmpty()) {
        QModelIndex index = ui->treeView->currentIndex();
        ModelPart *selectedPart = nullptr;

        // If no item is selected, add the new part as a child of the root item
        if (!index.isValid()) {
            selectedPart = partList->getRootItem();
        } else {
            selectedPart = static_cast<ModelPart*>(index.internalPointer());
        }

        // Create a new ModelPart for the STL file
        QList<QVariant> data = { QFileInfo(fileName).fileName(), "true" };
        QModelIndex newIndex = partList->appendChild(index, data);

        // Load the STL file into the new ModelPart
        ModelPart *newPart = static_cast<ModelPart*>(newIndex.internalPointer());
        newPart->loadSTL(fileName);

        // Update the renderer to show the new STL file
        updateRender();
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
    dialog.setModelPart(selectedPart);  // Pass the data into dialog

    connect(&dialog, &OptionDialog::deleteRequested, this, &MainWindow::onDeleteRequested);	// Connect the delete signal to the slot

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

        QAbstractItemModel *model = ui->treeView->model();
        emit model->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});

        
        ui->treeView->update();
        ui->treeView->viewport()->update();

		updateRender();
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
    OptionDialog dialog(this);
    dialog.setModelPart(selectedPart);

    if (dialog.exec() == QDialog::Accepted) {  // If user clicks OK
        emit statusUpdateMessage("Updated: " + selectedPart->data(0).toString(), 0);

        // Notify TreeView model to refresh
        ui->treeView->model()->dataChanged(index, index);
    }
}

void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    updateRenderFromTree(partList->index(0, 0, QModelIndex()));
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