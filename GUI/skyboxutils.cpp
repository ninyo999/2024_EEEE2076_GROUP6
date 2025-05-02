#include "skyboxutils.h"

#include <vtkSkybox.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageFlip.h>
#include <vtkOpenGLTexture.h>
#include <vtkRenderer.h>
#include <iostream>

vtkSmartPointer<vtkOpenGLTexture> LoadCubemapTexture(const std::vector<std::string>& faceFilenames) {

    auto texture = vtkSmartPointer<vtkOpenGLTexture>::New();
    texture->CubeMapOn();
    texture->SetUseSRGBColorSpace(true);
    texture->InterpolateOn();
    texture->RepeatOff();    // prevent seams
    texture->MipmapOff();
    for (int i = 0; i < 6; ++i) {
        vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
        vtkImageReader2* reader = readerFactory->CreateImageReader2(faceFilenames[i].c_str());

        if (!reader) {
            std::cerr << "Failed to load cubemap face: " << faceFilenames[i] << std::endl;
            continue;
        }

        reader->SetFileName(faceFilenames[i].c_str());
        reader->Update();

        // Flip the image vertically (optional depending on your source)
        auto flipY = vtkSmartPointer<vtkImageFlip>::New();
        flipY->SetInputConnection(reader->GetOutputPort());
        flipY->SetFilteredAxis(1); // Y-axis
        flipY->Update();

        texture->SetInputConnection(i, flipY->GetOutputPort());
        reader->Delete();
    }

    return texture;
}

void AddSkyboxToRenderer(vtkRenderer* renderer, vtkTexture* cubemapTexture) {
    auto skybox = vtkSmartPointer<vtkSkybox>::New();
    skybox->SetTexture(cubemapTexture);
    skybox->SetProjectionToCube();
    skybox->GammaCorrectOn();

    renderer->AddActor(skybox);
}
