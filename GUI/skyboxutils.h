#pragma once

#include <vector>
#include <string>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkOpenGLTexture.h>

vtkSmartPointer<vtkOpenGLTexture> LoadCubemapTexture(const std::vector<std::string>& faceFilenames);
void AddSkyboxToRenderer(vtkRenderer* renderer, vtkTexture* cubemapTexture);
