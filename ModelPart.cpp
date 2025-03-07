/**     @file ModelPart.cpp
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
  */

#include "ModelPart.h"
#include <QDebug>


/* Commented out for now, will be uncommented later when you have
 * installed the VTK library
 */
#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>


ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent )
    : m_itemData(data), m_parentItem(parent) {

    /* You probably want to give the item a default colour */
}


ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}


void ModelPart::appendChild( ModelPart* item ) {
    /* Add another model part as a child of this part
     * (it will appear as a sub-branch in the treeview)
     */
    item->m_parentItem = this;
    m_childItems.append(item);
}


ModelPart* ModelPart::child( int row ) {
    /* Return pointer to child item in row below this item.
     */
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    /* Count number of child items
     */
    return m_childItems.count();
}


int ModelPart::columnCount() const {
    /* Count number of columns (properties) that this item has.
     */
    return m_itemData.count();
}



QVariant ModelPart::data(int column, int role) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return m_itemData.at(column);
    }

    if (role == Qt::BackgroundRole) {
        if (partColor.isValid()) {
            qDebug() << "Applying background color to row" << row() << "Color:" << partColor;
            return QBrush(partColor);  // ✅ Ensure it returns a QBrush
        }
    }

    if (role == Qt::ForegroundRole) {
        return QBrush(Qt::black);  // Optional: Change text color if needed
    }

    return QVariant();
}






void ModelPart::set(int column, const QVariant &value) {
    /* Set the data associated with a column of this item
     */
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}


ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}


int ModelPart::row() const {
    /* Return the row index of this item, relative to it's parent.
     */
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

void ModelPart::setColor(const QColor &color) {
    partColor = color;
    qDebug() << "setColor() called with RGB: " << partColor.red() << partColor.green() << partColor.blue();
}






unsigned char ModelPart::getColourR() const { return partColor.red(); }
unsigned char ModelPart::getColourG() const { return partColor.green(); }
unsigned char ModelPart::getColourB() const { return partColor.blue(); }

void ModelPart::setName(const QString& newName) {
    set(0, newName);
}




QColor ModelPart::color() const {
    return partColor;
}


void ModelPart::setVisible(bool visible) {
    /* This is a placeholder function that you will need to modify if you want to use it */
    isVisible = visible;
    set(1, visible ? "true" : "false");    /* As the name suggests ... */
}

bool ModelPart::Visible() const{
    /* This is a placeholder function that you will need to modify if you want to use it */
    return isVisible;
    //return data(1).toString() == "true";
    /* As the name suggests ... */

}

void ModelPart::loadSTL(QString fileName) {
    // Create a VTK STL reader
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();

    // Create a mapper and link it to the STL reader
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    // Create an actor and link it to the mapper
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // Store the VTK objects in the ModelPart
    this->file = reader;
    this->mapper = mapper;
    this->actor = actor;

    // Set the color of the actor (optional)
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // Red color
}

vtkSmartPointer<vtkActor> ModelPart::getActor() const{
    return actor;
}
//vtkActor* ModelPart::getNewActor() {
/* This is a placeholder function that you will need to modify if you want to use it
     *
     * The default mapper/actor combination can only be used to render the part in
     * the GUI, it CANNOT also be used to render the part in VR. This means you need
     * to create a second mapper/actor combination for use in VR - that is the role
     * of this function. */


/* 1. Create new mapper */

/* 2. Create new actor and link to mapper */

/* 3. Link the vtkProperties of the original actor to the new actor. This means
      *    if you change properties of the original part (colour, position, etc), the
      *    changes will be reflected in the GUI AND VR rendering.
      *
      *    See the vtkActor documentation, particularly the GetProperty() and SetProperty()
      *    functions.
      */


/* The new vtkActor pointer must be returned here */
//    return nullptr;

//}
