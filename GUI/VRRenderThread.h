#ifndef VRRENDERTHREAD_H
#define VRRENDERTHREAD_H

#include <QThread>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>

class VRRenderThread : public QThread {
    Q_OBJECT

public:
    VRRenderThread(QObject *parent = nullptr);
    ~VRRenderThread();

    void addActor(vtkSmartPointer<vtkActor> actor);
    void stop();

protected:
    void run() override;

private:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> interactor;
    QList<vtkSmartPointer<vtkActor>> actors;
    bool running;
};

#endif // VRRENDERTHREAD_H