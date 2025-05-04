#ifndef VR_RENDER_THREAD_H
#define VR_RENDER_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QColor>

#include <vtkActor.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRCamera.h>
#include <vtkActorCollection.h>
#include <vtkLight.h>  
#include <vtkSkybox.h> 

/**
 * @file
 * This file contains the declaration of the VRRenderThread class,
 * which handles rendering of 3D content in VR using VTK's OpenVR module.
 */

 /**
  * @class VRRenderThread
  * @brief A thread for managing VR rendering using VTK and OpenVR.
  *
  * This class creates and manages a separate thread for VR rendering. It supports
  * adding actors, issuing rendering commands (e.g., rotation, lighting), and setting
  * a skybox or background color.
  */
class VRRenderThread : public QThread {
    Q_OBJECT

public:
    /**
     * @brief Rendering and control commands that can be issued to the VR thread.
     */
    enum {
        END_RENDER,          /**< Stops the VR rendering thread */
        ROTATE_X,            /**< Rotate the scene along the X-axis */
        ROTATE_Y,            /**< Rotate the scene along the Y-axis */
        ROTATE_Z,            /**< Rotate the scene along the Z-axis */
        SET_LIGHT_INTENSITY, /**< Adjust the lighting intensity */
        LOAD_SKYBOX          /**< Load a skybox texture */
    } Command;

    /**
     * @brief Constructs the VRRenderThread.
     * @param parent Parent QObject.
     */
    VRRenderThread(QObject* parent = nullptr);

    /**
     * @brief Destructor for VRRenderThread.
     */
    ~VRRenderThread() override;

    /**
     * @brief Adds an actor to the VR scene (before the thread starts).
     * @param actor The actor to add.
     */
    void addActorOffline(vtkActor* actor);

    /**
     * @brief Issues a command to the VR renderer.
     * @param cmd The command type.
     * @param value Associated value (e.g., rotation angle or light intensity).
     */
    void issueCommand(int cmd, double value);

    /**
     * @brief Loads a skybox cubemap for the VR scene.
     * @param faceFilenames A list of 6 image paths for the cubemap.
     */
    void loadSkybox(const std::vector<std::string>& faceFilenames);

public slots:
<<<<<<< HEAD
    /**
     * @brief Sets the VR scene background color.
     * @param color The background color.
     */
    void setVRBackgroundColor(const QColor& color);
=======
	void setVRBackgroundColor(const QColor& color);
>>>>>>> aa1abbf0f105ed0cac03f9bf88362994a1e75d43

protected:
    /**
     * @brief Main execution function for the thread.
     */
    void run() override;

private:
    vtkSmartPointer<vtkOpenVRRenderWindow> window; /**< VR render window */
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> interactor; /**< VR interactor */
    vtkSmartPointer<vtkOpenVRRenderer> renderer; /**< VR renderer */
    vtkSmartPointer<vtkOpenVRCamera> camera; /**< VR camera */

    QMutex mutex; /**< Mutex for thread-safe access */
    QWaitCondition condition; /**< Condition for command synchronization */

    vtkSmartPointer<vtkActorCollection> actors; /**< Actors to render */

    std::chrono::time_point<std::chrono::steady_clock> t_last; /**< Last update time */

    bool endRender; /**< Flag to end rendering loop */

    double rotateX; /**< Rotation speed on X-axis */
    double rotateY; /**< Rotation speed on Y-axis */
    double rotateZ; /**< Rotation speed on Z-axis */

    vtkSmartPointer<vtkSkybox> skybox; /**< Skybox for VR background */
    vtkSmartPointer<vtkLight> light; /**< Lighting in the scene */
    double lightIntensity; /**< Current light intensity */
};

#endif // VR_RENDER_THREAD_H
