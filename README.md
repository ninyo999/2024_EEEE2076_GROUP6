# Car model in VR (EEEE2076 Project)

This project is a desktop app that lets you open and view 3D models (in STL format) in a simple and interactive way. You can change the background, adjust lighting, and apply effects like cutting or shrinking parts of the model to see them better. It also has a special mode that works with VR headsets like HTC Vive, so you can view the 3D models in virtual reality. The app is built using Qt for the user interface and VTK for 3D graphics.

## Built with

* Qt for the user interface

* VTK for 3D graphics.

* Visual Studio 2022 for Windows builds

* CMake 

## GUI folder

`mainwindow.*` |   Main window class — UI, rendering, event logic
 
`ModelPart.*`      | Model node class (tree structure + VTK actor)  

 `ModelPartList.*`  | Qt model for tree view                         

 `optiondialog.*`   | Dialog for editing a model part               

`backgrounddialog.*` | Dialog for selecting background options     

 `skyboxutils.*`    | Utility functions for skybox loading           

 `vrrenderthread.*` | Threaded VR rendering logic using OpenVR       

 `main.cpp`         | Entry point of the application                 

 `icons.qrc`        | Qt resource file for loading icons            

 `*.ui`             | Qt Designer UI definitions                     

 `CMakeLists.txt`   | CMake configuration for building the GUI       

## Icons folder

This folder contains PNG or SVG icons used in the Qt GUI interface.



## Usage

After building and running the application, you can:

1. Load a 3D model by clicking “Open File” and selecting one or more .stl files.

2. The model(s) will appear in the tree view on the left side of the window.

3. Click on a model in the tree to:
 * Change its name, color, and visibility.

* Apply filters like Clip (cut part of it) or Shrink (shrink geometry).

4. Change the background by selecting a solid color or loading a custom image.

5. Adjust lighting and drift animation using the sliders.

6. Click “Start VR” to launch the scene in your VR headset and "Stop VR" to stop it.

### Example of application

![image](https://github.com/user-attachments/assets/25bf49d9-9d2f-4ed4-aaf6-b94dca1fbd95)

