<div align="center">

# Nebula Engine

**A small, cross-platform 2D/3D game engine and editor built in C++23.**

Nebula is a personal engine project focused on learning how modern game
technology fits together: rendering, scene data, asset loading, editor tools,
and platform abstraction.

<p>
  <a href="#showcase">Showcase</a> ·
  <a href="#features">Features</a> ·
  <a href="#building">Building</a> ·
  <a href="#project-status">Status</a>
</p>

</div>

## Showcase

<p align="center">
  <img src="Docs/Images/NebulaEditor.png" alt="Nebula Editor showing a scene hierarchy, viewport, properties panel, and content browser" width="100%">
</p>

<p align="center"><em>Nebula Editor — scene authoring, viewport editing, and asset browsing.</em></p>

<table>
  <tr>
    <td width="50%">
      <img src="Docs/Images/NebulaEngine2D.png" alt="Nebula Engine 2D rendering a tiled underground scene">
    </td>
    <td width="50%">
      <img src="Docs/Images/NebulaEngine3D.png" alt="Nebula Engine 3D rendering imported models and textured geometry">
    </td>
  </tr>
  <tr>
    <td align="center"><em>2D rendering</em></td>
    <td align="center"><em>3D rendering and model loading</em></td>
  </tr>
</table>

## Features

- **2D and 3D rendering** with batching, textures, cameras, meshes, and
  framebuffers.
- **Nebula Editor** with a scene hierarchy, viewport, transform gizmo,
  properties panel, and content browser.
- **Scene authoring** backed by an EnTT entity-component model and YAML scene
  files.
- **Model importing** through [Assimp](https://assimp.org/).
- **Shader support** through the [Slang](https://github.com/shader-slang/slang)
  shading language.
- **Platform backends** for Metal 3 and Vulkan.
- **Cross-platform foundation** targeting Windows, macOS, and Linux.

## Examples

The repository includes small, focused examples for exploring the engine:

| Example | What it demonstrates |
| --- | --- |
| `Renderer2DExample` | 2D rendering |
| `3DRendererExample` | 3D rendering and model loading |
| `BatchRendererExample` | Batched rendering |
| `PlatformMetal3Example` | Metal 3 platform layer |
| `PlatformVulkanExample` | Vulkan platform layer |

## Building

### Requirements

- [CMake](https://cmake.org/) **4.1.2 or newer**
- Ninja
- A C++23-compatible compiler
- Clang is recommended
- Platform graphics development dependencies:
  - **Windows:** Windows 10 SDK, Windows App SDK, NuGet and Vulkan SDK
  - **macOS:** Xcode and the macOS SDK
  - **Linux:** Vulkan validation layers

### Configure and build

From the repository root:

```bash
cmake --list-presets # Lists available build presets
cmake -S . -B build --preset <preset-name> # Configure the build
cmake --build build --preset <preset-name> # Build the project
```

The main targets are `NebulaEditor` and the renderer/platform examples. Built
executables are written under `bin/`.

> **Note:** Nebula is actively evolving. Build configuration and supported
> features may change while the engine is being developed.

## Project status

Nebula is currently a **learning project and engine prototype**, not a
production-ready game-making tool. The core rendering and editor foundations
are in place.

## Credits

Nebula was started while following
[TheCherno's Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).
The project has since diverged into its own architecture and experiments.

Nebula is built with:

[Assimp](https://assimp.org/) ·
[EnTT](https://github.com/skypjack/entt) ·
[Glad2](https://github.com/Dav1dde/glad/tree/glad2) ·
[GLM](https://github.com/g-truc/glm) ·
[Dear ImGui](https://github.com/ocornut/imgui) ·
[ImGuizmo](https://github.com/cedricguillemet/imguizmo) ·
[metal-cpp](https://github.com/apple/metal-cpp) ·
[nanosvg](https://github.com/memononen/nanosvg) ·
[SDL](https://www.libsdl.org/) ·
[Slang](https://github.com/shader-slang/slang) ·
[spdlog](https://github.com/gabime/spdlog) ·
[stb](https://github.com/nothings/stb) ·
[yaml-cpp](https://github.com/jbeder/yaml-cpp)
