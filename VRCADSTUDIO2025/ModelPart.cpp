#include "ModelPart.h"
#include <QDebug>
#include <QColor>
#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkSTLReader.h>
#include <vtkRenderer.h>
#include <vtkClipDataSet.h>
#include <vtkShrinkFilter.h>
#include <vtkPlane.h>
#include <vtkGeometryFilter.h>

/**
 * @brief Constructs a ModelPart with initial data and optional parent.
 * Initializes default color, visibility, and filter states.
 */
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent) {
    partColor = QColor(255, 255, 255);
    isVisible = true;
    clipEnabled = false;
    shrinkEnabled = false;
    shrinkFactor = 0.8;
    clipOrigin[0] = clipOrigin[1] = clipOrigin[2] = 0.0;
    clipNormal[0] = -1.0; clipNormal[1] = 0.0; clipNormal[2] = 0.0;
}

/**
 * @brief Destructor: deletes all child parts.
 */
ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

/**
 * @brief Adds a child part to this part.
 */
void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;
    m_childItems.append(item);
}

ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    return m_childItems.count();
}

int ModelPart::columnCount() const {
    return m_itemData.count();
}

/**
 * @brief Returns data based on the column and role (display, background, etc.).
 */
QVariant ModelPart::data(int column, int role) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return m_itemData.at(column);
    }

    if (role == Qt::BackgroundRole && partColor.isValid()) {
        qDebug() << "Applying background color to row" << row() << "Color:" << partColor;
        return QBrush(partColor);
    }

    if (role == Qt::ForegroundRole) {
        return QBrush(Qt::black);
    }

    return QVariant();
}

void ModelPart::set(int column, const QVariant& value) {
    if (column < 0 || column >= m_itemData.size())
        return;
    m_itemData.replace(column, value);
}

ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

/**
 * @brief Sets the part's color and applies it to the actor.
 */
void ModelPart::setColor(const QColor& color) {
    partColor = color;
    if (actor) {
        actor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
    }
    qDebug() << "setColor() called with RGB: " << partColor.red() << partColor.green() << partColor.blue();
}

unsigned char ModelPart::getColourR() const { return partColor.red(); }
unsigned char ModelPart::getColourG() const { return partColor.green(); }
unsigned char ModelPart::getColourB() const { return partColor.blue(); }

void ModelPart::setName(const QString& newName) {
    set(0, newName);
}

void ModelPart::setRenderer(vtkRenderer* renderer) {
    this->renderer = renderer;
}

/**
 * @brief Removes a child and deletes its resources.
 */
void ModelPart::removeChild(ModelPart* child) {
    if (m_childItems.contains(child)) {
        if (child->actor && child->renderer) {
            child->renderer->RemoveActor(child->actor);
        }
        m_childItems.removeOne(child);
        delete child;
    }
}

QColor ModelPart::color() const {
    return partColor;
}

void ModelPart::setVisible(bool visible) {
    isVisible = visible;
    set(1, visible ? "true" : "false");

    if (actor) {
        actor->SetVisibility(visible);
    }
}

bool ModelPart::visible() const {
    return isVisible;
}

/**
 * @brief Loads an STL file into this part and sets up its VTK actor.
 */
void ModelPart::loadSTL(QString fileName) {
    filePath = fileName;
    source = vtkSmartPointer<vtkSTLReader>::New();
    source->SetFileName(fileName.toStdString().c_str());
    source->Update();

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(source->GetOutputPort());

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(partColor.redF(), partColor.greenF(), partColor.blueF());
    actor->SetVisibility(isVisible);

    storeOriginalData();
}

vtkSmartPointer<vtkPolyData> ModelPart::getPolyData() const {
    if (!source) return nullptr;
    return source->GetOutput();
}

bool ModelPart::hasOriginalData() const {
    return originalData != nullptr;
}

void ModelPart::storeOriginalData() {
    if (source) {
        originalData = vtkSmartPointer<vtkPolyData>::New();
        originalData->DeepCopy(source->GetOutput());
    }
}

vtkSmartPointer<vtkPolyData> ModelPart::getOriginalPolyData() const {
    return originalData;
}

vtkSmartPointer<vtkActor> ModelPart::getActor() const {
    return actor;
}

/**
 * @brief Applies a clipping filter to this part.
 * @param enable Whether to enable the filter.
 * @param origin Origin point of the clipping plane.
 * @param normal Normal vector of the clipping plane.
 */
void ModelPart::applyClipFilter(bool enable, double origin[3], double normal[3]) {
    clipEnabled = enable;
    if (enable) {
        for (int i = 0; i < 3; i++) {
            clipOrigin[i] = origin[i];
            clipNormal[i] = normal[i];
        }
    }
    updateFilters();
}

/**
 * @brief Applies a shrink filter to this part.
 * @param enable Whether to enable shrinking.
 * @param factor Shrink factor to apply.
 */
void ModelPart::applyShrinkFilter(bool enable, double factor) {
    shrinkEnabled = enable;
    shrinkFactor = factor;
    updateFilters();
}

/**
 * @brief Updates the internal filter pipeline for this part.
 * Applies shrink and/or clip filters and updates the actor.
 */
void ModelPart::updateFilters() {
    if (!source || !mapper) return;

    vtkSmartPointer<vtkPolyData> processedData = vtkSmartPointer<vtkPolyData>::New();
    processedData->DeepCopy(originalData);

    // Shrink
    if (shrinkEnabled) {
        if (!shrinkFilter) {
            shrinkFilter = vtkSmartPointer<vtkShrinkFilter>::New();
        }
        shrinkFilter->SetInputData(processedData);
        shrinkFilter->SetShrinkFactor(shrinkFactor);
        shrinkFilter->Update();

        vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
        geometryFilter->SetInputConnection(shrinkFilter->GetOutputPort());
        geometryFilter->Update();

        processedData->DeepCopy(geometryFilter->GetOutput());
    }

    // Clip
    if (clipEnabled) {
        if (!clipFilter) {
            clipFilter = vtkSmartPointer<vtkClipDataSet>::New();
        }
        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
        plane->SetOrigin(clipOrigin);
        plane->SetNormal(clipNormal);

        clipFilter->SetInputData(processedData);
        clipFilter->SetClipFunction(plane);
        clipFilter->Update();

        vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
        geometryFilter->SetInputConnection(clipFilter->GetOutputPort());
        geometryFilter->Update();

        processedData->DeepCopy(geometryFilter->GetOutput());
    }

    mapper->SetInputData(processedData);
    mapper->Update();
}

bool ModelPart::isClipFilterEnabled() const {
    return clipEnabled;
}

bool ModelPart::isShrinkFilterEnabled() const {
    return shrinkEnabled;
}

/**
 * @brief Creates a new actor clone for use in VR rendering.
 * Copies properties, orientation, and color.
 */
vtkSmartPointer<vtkActor> ModelPart::getNewActor() const {
    if (!this->source)
        return nullptr;

    auto mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(source->GetOutputPort());

    auto actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);
    actor2->SetVisibility(this->isVisible);
    actor2->SetPosition(this->actor->GetPosition());
    actor2->SetOrientation(this->actor->GetOrientation());
    actor2->GetProperty()->DeepCopy(this->actor->GetProperty());

    return actor2;
}
