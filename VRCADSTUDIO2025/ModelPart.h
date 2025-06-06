#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QColor>
#include <QBrush>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkClipDataSet.h>
#include <vtkShrinkFilter.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkGeometryFilter.h>

/**
 * @file
 * @brief Defines the ModelPart class used to represent and manage components
 *        of a 3D model in a tree structure, including appearance, visibility,
 *        filters, and VTK rendering data.
 */

 /**
  * @class ModelPart
  * @brief Represents a part of a 3D model with support for hierarchy, rendering, and filtering.
  *
  * Each ModelPart holds its own VTK actor, color, visibility, and may have child parts.
  * It supports loading STL files, applying clip and shrink filters, and integration with a tree view.
  */
class ModelPart {
public:
    /**
     * @brief Constructs a ModelPart with given data and optional parent.
     */
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);

    /**
     * @brief Destructor. Deletes all child parts.
     */
    ~ModelPart();

    // --- Tree structure operations ---

    /**
     * @brief Appends a child to this ModelPart.
     */
    void appendChild(ModelPart* item);

    /**
     * @brief Returns the child at the given row.
     */
    ModelPart* child(int row);

    /**
     * @brief Returns the number of children.
     */
    int childCount() const;

    /**
     * @brief Returns the number of data columns.
     */
    int columnCount() const;

    /**
     * @brief Returns data for a given column and role (e.g., display, background).
     */
    QVariant data(int column, int role = Qt::DisplayRole) const;

    /**
     * @brief Sets data at a specific column.
     */
    void set(int column, const QVariant& value);

    /**
     * @brief Returns the parent item of this part.
     */
    ModelPart* parentItem();

    /**
     * @brief Returns the row index of this part in its parent.
     */
    int row() const;

    // --- Appearance properties ---

    /**
     * @brief Sets the display color of the part.
     */
    void setColor(const QColor& color);

    /**
     * @brief Gets the red color component.
     */
    unsigned char getColourR() const;

    /**
     * @brief Gets the green color component.
     */
    unsigned char getColourG() const;

    /**
     * @brief Gets the blue color component.
     */
    unsigned char getColourB() const;

    /**
     * @brief Returns the full QColor value.
     */
    QColor color() const;

    /**
     * @brief Sets the visibility state.
     */
    void setVisible(bool visible);

    /**
     * @brief Returns whether the part is visible.
     */
    bool visible() const;

    /**
     * @brief Sets the display name of the part.
     */
    void setName(const QString& newName);

    // --- VTK operations ---

    /**
     * @brief Loads an STL file and creates a VTK actor.
     */
    void loadSTL(QString fileName);

    /**
     * @brief Returns the actor for rendering.
     */
    vtkSmartPointer<vtkActor> getActor() const;

    /**
     * @brief Creates a new duplicate actor for use in another scene (e.g., VR).
     */
    vtkSmartPointer<vtkActor> getNewActor() const;

    /**
     * @brief Sets the renderer for this part.
     */
    void setRenderer(vtkRenderer* renderer);

    /**
     * @brief Removes and deletes a child part.
     */
    void removeChild(ModelPart* child);

    /**
     * @brief Returns the current poly data.
     */
    vtkSmartPointer<vtkPolyData> getPolyData() const;

    // --- Filter operations ---

    /**
     * @brief Enables or disables a clip filter and sets its parameters.
     */
    void applyClipFilter(bool enable, double origin[3], double normal[3]);

    /**
     * @brief Enables or disables a shrink filter and sets its factor.
     */
    void applyShrinkFilter(bool enable, double factor);

    /**
     * @brief Checks if the clip filter is enabled.
     */
    bool isClipFilterEnabled() const;

    /**
     * @brief Checks if the shrink filter is enabled.
     */
    bool isShrinkFilterEnabled() const;

    /**
     * @brief Checks if original (unfiltered) data is available.
     */
    bool hasOriginalData() const;

    /**
     * @brief Stores a deep copy of the original poly data.
     */
    void storeOriginalData();

    /**
     * @brief Returns the original, unmodified poly data.
     */
    vtkSmartPointer<vtkPolyData> getOriginalPolyData() const;

private:
    /**
     * @brief Internal function to update the VTK filter pipeline.
     */
    void updateFilters();

    // --- Internal members ---
    QList<ModelPart*> m_childItems;              /**< Child parts in hierarchy */
    QList<QVariant> m_itemData;                  /**< UI data per column */
    ModelPart* m_parentItem;                     /**< Parent in hierarchy */
    QColor partColor;                            /**< Color used for rendering */
    bool isVisible;                              /**< Visibility flag */
    QString filePath;                            /**< Path to STL file */

    // --- VTK-related objects ---
    vtkSmartPointer<vtkSTLReader> source;        /**< STL reader */
    vtkSmartPointer<vtkPolyDataMapper> mapper;   /**< Mapper for rendering */
    vtkSmartPointer<vtkActor> actor;             /**< Actor representing this part */
    vtkSmartPointer<vtkRenderer> renderer;       /**< Associated renderer */
    vtkSmartPointer<vtkPolyData> originalData;   /**< Backup of original geometry */

    // --- Filter-related members ---
    vtkSmartPointer<vtkClipDataSet> clipFilter;  /**< Clipping filter */
    vtkSmartPointer<vtkShrinkFilter> shrinkFilter; /**< Shrinking filter */
    bool clipEnabled;                            /**< Is clipping enabled */
    bool shrinkEnabled;                          /**< Is shrinking enabled */
    double shrinkFactor;                         /**< Factor for shrinking */
    double clipOrigin[3];                        /**< Origin of clip plane */
    double clipNormal[3];                        /**< Normal of clip plane */
};

#endif // VIEWER_MODELPART_H
