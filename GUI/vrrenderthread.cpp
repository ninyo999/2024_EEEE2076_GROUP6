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

    auto vrRenderer = vtkSmartPointer<vtkOpenVRRenderer>::New();
    auto vrRenderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
    auto vrInteractor = vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();

    vrRenderWindow->AddRenderer(vrRenderer);
    vrInteractor->SetRenderWindow(vrRenderWindow);

    // Copy actors from the main scene
    vtkActorCollection* actors = sceneRenderer->GetActors();
    actors->InitTraversal();
    while (vtkActor* actor = actors->GetNextActor()) {
        vrRenderer->AddActor(actor);
    }

    vrRenderer->SetBackground(sceneRenderer->GetBackground());

    vrRenderWindow->Render();
    vrInteractor->Start();
}
