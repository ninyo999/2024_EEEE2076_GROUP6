#pragma once

#include <vector>
#include <string>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkOpenGLTexture.h>

/**
 * @file
 * This file contains utility functions for loading a cubemap texture
 * and adding a skybox to a VTK renderer.
 */

 /**
  * @brief Loads a cubemap texture from six image files.
  *
  * This function loads six images representing the faces of a cubemap and
  * returns a VTK OpenGL texture that can be used in a skybox.
  *
  * @param faceFilenames A vector of 6 file paths in the order:
  *        right, left, top, bottom, front, back.
  * @return A smart pointer to the loaded cubemap texture.
  */
vtkSmartPointer<vtkOpenGLTexture> LoadCubemapTexture(const std::vector<std::string>& faceFilenames);

/**
 * @brief Adds a skybox to the specified VTK renderer using the provided cubemap texture.
 *
 * This function creates and attaches a skybox actor using the given texture.
 *
 * @param renderer Pointer to the VTK renderer.
 * @param cubemapTexture The cubemap texture to use for the skybox.
 */
void AddSkyboxToRenderer(vtkRenderer* renderer, vtkTexture* cubemapTexture);
