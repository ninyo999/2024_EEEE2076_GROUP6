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

    // Create new VR render window and interactor
    auto vrRenderer = vtkSmartPointer<vtkOpenVRRenderer>::New();
    auto vrRenderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
    auto vrInteractor = vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();

    vrRenderWindow->AddRenderer(vrRenderer);
    vrInteractor->SetRenderWindow(vrRenderWindow);

    // --- Duplicate the Scene ---
    if (sceneRenderer)
    {
        vtkActorCollection* actors = sceneRenderer->GetActors();
        actors->InitTraversal();

        while (vtkActor* actor = actors->GetNextActor())
        {
            auto newActor = vtkSmartPointer<vtkActor>::New();
            newActor->ShallowCopy(actor);  // <-- shallow copy actor
            vrRenderer->AddActor(newActor);
        }

        vrRenderer->SetBackground(sceneRenderer->GetBackground());
    }

    vrRenderWindow->Initialize();

    if (vrRenderWindow->GetHMD() == nullptr)
    {
        std::cerr << "Failed to initialize VR (HMD not detected)!" << std::endl;
        std::cout << "[VR] No HMD found\n";
		return;
    }
	else
	{
		std::cout << "[VR] HMD detected!\n";
	}

	vrRenderWindow->SetSize(1280, 720);
	vrRenderWindow->SetWindowName("VR Desktop Preview");
    
	vrRenderWindow->SetWindowName("VR Window");
    vrRenderWindow->Render();
    vrInteractor->Start();  // Infinite loop inside thread
}
