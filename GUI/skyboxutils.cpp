#include "skyboxutils.h"
#include <vtkSkybox.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageFlip.h>
#include <vtkOpenGLTexture.h>
#include <vtkRenderer.h>
#include <iostream>

/**
 * @brief Loads a cubemap texture from six image files.
 *
 * This function creates a VTK OpenGL cubemap texture using six image files.
 * Each image is flipped on the Y-axis to match OpenGL's texture coordinate system.
 *
 * @param faceFilenames A vector of 6 file paths for the cube faces, in this order:
 *        +X (right), -X (left), +Y (top), -Y (bottom), +Z (front), -Z (back).
 * @return A smart pointer to the resulting vtkOpenGLTexture.
 */
vtkSmartPointer<vtkOpenGLTexture> LoadCubemapTexture(const std::vector<std::string>& faceFilenames) {
    auto texture = vtkSmartPointer<vtkOpenGLTexture>::New();
    texture->CubeMapOn();
    texture->SetUseSRGBColorSpace(true);
    texture->InterpolateOn();
    texture->RepeatOff();
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

 
        auto flipY = vtkSmartPointer<vtkImageFlip>::New();
        flipY->SetInputConnection(reader->GetOutputPort());
        flipY->SetFilteredAxis(1); 
        flipY->Update();

        texture->SetInputConnection(i, flipY->GetOutputPort());
        reader->Delete();
    }

    return texture;
}

/**
 * @brief Adds a skybox actor to a VTK renderer using the provided cubemap texture.
 *
 * @param renderer Pointer to the target vtkRenderer.
 * @param cubemapTexture The cubemap texture to use for the skybox.
 */
void AddSkyboxToRenderer(vtkRenderer* renderer, vtkTexture* cubemapTexture) {
    auto skybox = vtkSmartPointer<vtkSkybox>::New();
    skybox->SetTexture(cubemapTexture);
    skybox->SetProjectionToCube();
    skybox->GammaCorrectOn();

    renderer->AddActor(skybox);
}
