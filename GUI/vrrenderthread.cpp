#include "VRRenderThread.h"

#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkActorCollection.h>
#include <vtkActor.h>

VRRenderThread::VRRenderThread(QObject* parent)
    : QThread(parent), running(false)
{
}

VRRenderThread::~VRRenderThread()
{
    stop();
}

void VRRenderThread::setSceneData(vtkSmartPointer<vtkRenderer> renderer)
{
    sceneRenderer = renderer;
}

void VRRenderThread::stop()
{
    running = false;
}

void VRRenderThread::run()
{
    running = true;

    // Create OpenVR Renderer, RenderWindow, Interactor
    vtkSmartPointer<vtkOpenVRRenderer> vrRenderer = vtkSmartPointer<vtkOpenVRRenderer>::New();
    vtkSmartPointer<vtkOpenVRRenderWindow> vrRenderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> vrInteractor = vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();

    vrRenderWindow->AddRenderer(vrRenderer);
    vrInteractor->SetRenderWindow(vrRenderWindow);

    // Copy actors from the main scene
    if (sceneRenderer)
    {
        vtkActorCollection* actors = sceneRenderer->GetActors();
        actors->InitTraversal();
        while (vtkActor* actor = actors->GetNextActor()) {
            vrRenderer->AddActor(actor);
        }

        vrRenderer->SetBackground(sceneRenderer->GetBackground());
    }

    // ? Here is the trick ?
    // Manually create a render window (desktop monitor) even without VR headset
    vrRenderWindow->SetWindowName("Virtual Reality Render Window");
    vrRenderWindow->SetSize(1000, 800);  // Nice desktop window size
    vrRenderWindow->Render();

    vrInteractor->Initialize();
    vrInteractor->Start();  // This starts the event loop (makes window visible and interactive)

    running = false;
}
