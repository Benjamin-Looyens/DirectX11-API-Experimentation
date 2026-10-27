# DirectX11 Hardware Rasterizer
Simple DirectX11 setup with SDL2 for the course "Graphics Programming" in Howest DAE Game Development. The project implements the primary rasterizer pipeline (`Input assembler` -> `Vertex Shader` -> `Geometry Shader` -> `Rasterization` -> `Pixel Shader` -> `Output Merger` -> `Render Target` -> `Swapchain`). On top of this, the setup contains a [software rasterizer](https://github.com/Benjamin-Looyens/Software-Rasterizer) that was implemented separately. The software rasterizer runs entirely on the CPU, whereas as expected the DirectX part runs on the GPU.

## Features
- Perspective camera:
    - WASD for 360 movement
    - RMB for camera shifting
    - LMB for diving (forward + camera)
    - RMB + LMB for camera panning
    - R for setting position to starting one
- Rendering
    - F7 toggles depth buffer render (for Software)
    - F8 toggles tri bounding boxes render (for Software)
    - F5 toggles shading mode (for Software)
- Mesh control
    - F2 toggles mesh rotation (both Software & DirectX)
    - F6 toggles mesh normal map usage (for Software)
    - F3 toggles the transparent fire mesh (for DirectX)
    - F4 toggles the texture sampling method (for DirectX)

## Examples
<img width="642" height="514" alt="DIrectX" src="https://github.com/user-attachments/assets/ba2bce20-8345-417d-920b-04059ad2086e" />
<img width="642" height="514" alt="Rasterizer" src="https://github.com/user-attachments/assets/79bfc551-ec43-437a-a999-de1a5737d73b" />
