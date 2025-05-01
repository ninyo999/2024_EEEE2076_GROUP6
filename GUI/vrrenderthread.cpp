/**		@file VRRenderThread.cpp
  *
  *		EEEE2046 - Software Engineering & VR Project
  *
  *		Template to add VR rendering to your application.
  *
  *		P Evans 2022
  */

#include "VRRenderThread.h"

#include <QMutexLocker>
#include <QOffscreenSurface>
#include <QOpenGLContext>
/* Vtk headers */
#include <vtkActor.h>
#include <vtkOpenVRRenderWindow.h>				
#include <vtkOpenVRRenderWindowInteractor.h>	
#include <vtkOpenVRRenderer.h>					
#include <vtkOpenVRCamera.h>	

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSTLReader.h>
#include <vtkDataSetmapper.h>
#include <vtkCallbackCommand.h>


/* The class constructor is called by MainWindow and runs in the primary program thread, this thread
 * will go on to handle the GUI (mouse clicks, etc). The OpenVRRenderWindowInteractor cannot be start()ed
 * in the constructor, as it will take control of the main thread to handle the VR interaction (headset 
 * rotation etc. This means that a second thread is needed to handle the VR.
 */
VRRenderThread::VRRenderThread( QObject* parent ) {
	/* Initialise actor list */
	actors = vtkActorCollection::New();

	/* Initialise command variables */
	rotateX = 0.;
	rotateY = 0.;
	rotateZ = 0.;
}


/* Standard destructor - this is important here as the class will be destroyed when the user
 * stops the VR thread, and recreated when the user starts it again. If class variables are 
 * not deallocated properly then there will be a memory leak, where the program's total memory
 * usage will increase for each start/stop thread cycle.
 */
VRRenderThread::~VRRenderThread() {

}


void VRRenderThread::addActorOffline( vtkActor* actor ) {

	/* Check to see if render thread is running */
	if (!this->isRunning()) {
		actors->AddItem(actor);
	}
}



void VRRenderThread::issueCommand( int cmd, double value ) {

	/* Update class variables according to command */
	switch (cmd) {
		/* These are just a few basic examples */
		case END_RENDER:
			this->endRender = true;
			break;

		case ROTATE_X:
			this->rotateX = value;
			break;

		case ROTATE_Y:
			this->rotateY = value;
			break;

		case ROTATE_Z:
			this->rotateZ = value;
			break;
	}
}

/* This function runs in a separate thread. This means that the program 
 * can fork into two separate execution paths. This thread is triggered by
 * calling VRRenderThread::start()
 */

void VRRenderThread::run()
{
    // ??? 0) Set up a completely independent GL context + surface ??????????
    QSurfaceFormat fmt;
    fmt.setMajorVersion(4);
    fmt.setMinorVersion(1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    QOffscreenSurface offscreenSurface;
    offscreenSurface.setFormat(fmt);
    offscreenSurface.create();
    if (!offscreenSurface.isValid()) {
        qWarning("Failed to create offscreen surface for VR!");
        return;
    }

    QOpenGLContext offscreenContext;
    offscreenContext.setFormat(offscreenSurface.format());
    if (!offscreenContext.create()) {
        qWarning("Failed to create offscreen GL context for VR!");
        return;
    }

    if (!offscreenContext.makeCurrent(&offscreenSurface)) {
        qWarning("Failed to make offscreen GL context current in VR thread!");
        return;
    }

    // ??? 1) Build the VTK pipeline ???????????????????????????????????????
    vtkNew<vtkNamedColors> colors;
    std::array<unsigned char,4> bkg{{26,51,102,255}};
    colors->SetColor("BkgColor", bkg.data());

    renderer = vtkOpenVRRenderer::New();
    renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());

    // Add any actors from actors->InitTraversal() …

    // ??? 2) Create the VR window WITHOUT sharing Qt’s widget context ????
    window = vtkOpenVRRenderWindow::New();
    window->SetSharedRenderWindow(nullptr);    // disable any context sharing
    window->Initialize();
    window->AddRenderer(renderer);

    // ??? 3) Set up camera & interactor ???????????????????????????????
    camera = vtkOpenVRCamera::New();
    renderer->SetActiveCamera(camera);

    interactor = vtkOpenVRRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    interactor->Initialize();

    // First frame
    window->Render();

    // ??? 4) VR loop ??????????????????????????????????????????????????
    endRender = false;
    t_last    = std::chrono::steady_clock::now();

    while (!interactor->GetDone() && !endRender) {
        interactor->DoOneEvent(window, renderer);

        // … your animation or updatePending logic …

        // swap buffers & finish
        window->Render();
    }

    // ??? 5) Clean up your offscreen context ???????????????????????????
    offscreenContext.doneCurrent();
    // (Qt will destroy the offscreenSurface/context on thread exit)
}
void VRRenderThread::updateActors(const std::vector<vtkSmartPointer<vtkActor>>& newActors)
{
    QMutexLocker lock(&mutex);
    pendingActors = newActors;
    updatePending = true;
}