#ifndef VRRENDERTHREAD_H
#define VRRENDERTHREAD_H

#include <QThread>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

class VRRenderThread : public QThread
{
    Q_OBJECT

public:
    explicit VRRenderThread(QObject* parent = nullptr);
    ~VRRenderThread();

    void setSceneData(vtkSmartPointer<vtkRenderer> renderer);
    void stop();

protected:
    void run() override;

private:
    vtkSmartPointer<vtkRenderer> sceneRenderer;
    bool running;
};

#endif // VRRENDERTHREAD_H
