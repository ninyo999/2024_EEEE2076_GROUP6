#include "VRRenderThread.h"
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>

VRRenderThread::VRRenderThread(QObject *parent)
    : QThread(parent), running(false) {
    renderer = vtkSmartPointer<vtkOpenVRRenderer>::New();
    renderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
    interactor = vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();

    renderWindow->AddRenderer(renderer);
    interactor->SetRenderWindow(renderWindow);
}

VRRenderThread::~VRRenderThread() {
    stop();
}

void VRRenderThread::addActor(vtkSmartPointer<vtkActor> actor) {
    actors.append(actor);
    renderer->AddActor(actor);
}

void VRRenderThread::stop() {
    if (running) {
        running = false;
        interactor->TerminateApp();
        quit();
        wait();
    }
}

void VRRenderThread::run() {
    running = true;
    renderWindow->Render();
    interactor->Start();
}