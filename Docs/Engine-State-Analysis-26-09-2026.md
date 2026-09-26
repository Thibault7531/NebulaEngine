# Nebula Engine: State Analysis

**Assessment date:** 2026-09-26  
**Scope:** Engine, platform backends, editor, examples, build configuration, and available test discovery in this checkout.

## Executive assessment

Nebula is a promising, actively evolving **engine and editor prototype**, not yet a dependable general-purpose game-making tool. It already has real rendering and authoring foundations: a graphics-device abstraction, 2D batching, a basic ECS scene model, scene YAML files, a dockable editor, and OpenGL/Metal backends. The editor can create and inspect simple scenes, and the examples demonstrate both 2D and low-level 3D drawing.

The main gap is not another renderer feature: it is a reliable end-to-end workflow for making, saving, running, and packaging a small game. Scene persistence currently loses important data; runtime scripting is not connected to an editor play loop; the scene renderer handles sprites rather than the imported 3D models; and there are no project-owned automated tests. Several unchecked input and resource paths can also cause crashes or undefined behavior.

**Recommendation:** focus the next milestone on a small, complete 2D game workflow and its reliability. Avoid expanding all platforms and both 2D/3D feature sets in parallel until a testable, distributable vertical slice works from editor authoring through runtime.

## What is present

| Area | Implemented capability | Current boundary |
|---|---|---|
| Platform and application | C++23 application/layer loop, window and event abstractions, input APIs, file dialogs, logging, profiling hooks | Platform code is split across Linux/SDL, Windows/WinRT, and macOS/Swift; cross-platform behavior is not backed by project tests in this checkout. |
| Graphics | Device, buffers, textures, framebuffers, render passes, pipelines, command buffers, uniforms, blend/depth/cull controls | OpenGL 4.6 is used on Linux/Windows and Metal 3 on macOS; backend behavior and feature parity need explicit verification. |
| Shaders | Slang shader compilation/reflection, with sample shaders | No shader authoring workflow, hot reload, or robust compile-error recovery is evident. |
| 2D rendering | Batched quads, color/texture/tiling support, texture slots, entity IDs for editor picking | The scene renderer renders sprites only; advanced 2D needs (for example, sorting/layers and animation) are not present as integrated scene features. |
| 3D rendering and assets | Assimp model import; mesh vertex/index buffers; 3D camera and direct-render examples | Imported models are not integrated into scene components or the scene renderer. The importer does not construct materials or textures from the model, and animation is not implemented. |
| Scenes and entities | EnTT-backed entities; tag, transform, sprite, camera, and native-script components; primary-camera selection | Flat entity listing rather than parent/child scene hierarchy; no UUID-backed identity, prefab workflow, or complete component persistence. |
| Scene files | YAML serialization/deserialization for tags, transforms, sprite color, and camera settings | Texture path and tiling factor are omitted; entity identity is not preserved; runtime serialization methods are unimplemented. |
| Editor | ImGui panels, scene viewport, transform gizmo, hierarchy/properties, scene open/save, asset browser, scene/texture drag-and-drop, themes | No play/stop workflow, undo/redo, full asset inspection/import pipeline, or reliable recovery from invalid assets/scenes. |
| Examples | 2D, 3D, batch rendering, OpenGL platform and Metal platform examples | Useful for experimentation, but they do not substitute for a game sample or regression suite. |

The project README describes Nebula as a learning project and documents the broad platform/rendering goals. The newest visible commits are editor content-browser improvements, which is positive evidence of continued iteration.

## What is working in its favor

- **A meaningful foundation exists.** This is more than a window and a triangle: graphics resources, scene entities, an editor viewport, scene files, and examples are all present.
- **The rendering API has a useful seam.** The device/render-pass abstraction separates much engine-facing code from OpenGL and Metal implementations, making backend-specific work possible without rewriting all callers.
- **There is a usable first 2D authoring loop in the editor.** Users can create entities, edit basic transforms/components, select entities in the viewport, and browse/drop textures.
- **Libraries are chosen pragmatically.** EnTT, Assimp, ImGui, ImGuizmo, Slang, and YAML cover substantial low-level needs and avoid reinventing broad infrastructure.
- **There is room to grow from examples.** The batch example and 3D samples expose low-level rendering capabilities that can later be promoted into scene-level features.

## Readiness and key gaps

### Release-blocking workflow gaps

1. **Scene files are not yet trustworthy.** `SceneSerializer::SerializeEntity` writes every entity ID as `"0"`; deserialization reads the ID but does not restore it. Sprite texture references and tiling are not serialized. Runtime serialization/deserialization explicitly assert as unimplemented. This makes round-tripping a scene lossy and prevents reliable references between entities.
2. **There is no integrated game runtime loop.** `Scene::OnUpdateRuntime` exists, but the application/editor do not call it. The editor always renders through `RenderSceneEditor`; there is no visible play/stop transition or player/export target. A game author cannot validate the actual runtime behavior from the editor workflow.
3. **Native-script ownership and lifecycle are incomplete.** Script factory/destructor function pointers are not initialized by default, while runtime update calls the factory when an instance is missing. `DestroyScript` and `OnDestroy` are not invoked by scene/entity destruction. A script component must be configured correctly to avoid calling an indeterminate pointer, and initialized instances can leak.
4. **Scene-level 3D support is absent.** Assimp model import and direct 3D drawing exist, but the scene component set and `SceneRenderer` only render sprites. A 3D game would need model/material components, scene rendering integration, and a tested asset path before the examples amount to an editor-driven 3D workflow.
5. **No project-owned test suite was found.** CTest discovers zero tests for the current build directory, and no engine/editor tests or project CI workflow were found among tracked project files. Core serialization, entity lifecycle, graphics contracts, and platform-specific behavior therefore lack automated regression coverage.

### Concrete risks and likely bugs

| Priority | Finding | Why it matters / source |
|---|---|---|
| **High** | Viewport picking indexes the integer attachment without validating mouse coordinates against the attachment dimensions. | A stale hover state, edge rounding, resize timing, or out-of-bounds position can produce an invalid vector index. The full attachment is also read back on every click, which can be an expensive GPU/CPU synchronization. `NebulaEditor/source/EditorLayer.cpp` (`OnMouseButtonPressedEvent`). |
| **High** | Scene serialization does not preserve unique entity IDs and omits sprite texture and tiling data. | Entity references cannot be stable across save/load, and textured scenes reopen differently from how they were authored. `NebulaEngine/source/Core/Nebula/Scene/SceneSerializer.cpp` (`SerializeEntity`, `Deserialize`). |
| **High** | Native script callbacks are uninitialized until `Bind<T>()`, and script destruction callbacks are not wired into scene/entity destruction. | Unconfigured scripts can invoke an indeterminate function pointer; configured script instances may leak and miss their destruction hook. `NebulaEngine/source/Core/Nebula/Scene/Components.h`, `Scene.cpp`, and `ScriptableEntity.h`. |
| **High** | Model import asserts on failure but then continues to dereference the returned scene; it also reads `mNormals` without ensuring normals exist. | In non-assert builds, invalid model files can crash. Valid meshes without normals can also be dereferenced incorrectly. Import flags or explicit validation/generation are needed. `NebulaEngine/source/Core/Nebula/Renderer/Model/Model.cpp`. |
| **Medium** | Camera aspect ratio divides by viewport height without guarding zero dimensions. | A zero-sized/minimized viewport can generate an infinite/invalid projection matrix. Both scene and editor cameras have this pattern. `SceneCamera.cpp` and `EditorCamera.cpp`. |
| **Medium** | Scene deserialization does not check that the file opened and does not translate YAML/conversion exceptions into a useful failure result. | Missing, truncated, or malformed files can escape as exceptions instead of a controlled editor error. The editor creates a fresh scene before loading, so a failed open can also discard the current in-memory scene. `SceneSerializer.cpp`, `EditorLayer.cpp` (`OpenScene`). |
| **Medium** | Content-browser root is constructed from the process working directory (`absolute("assets")`). | The editor can start in the wrong directory or throw during directory iteration when launched with a different working directory or when that folder is absent. Use the project/resource root and report filesystem errors. `ContentBrowserPanel.cpp`. |
| **Medium** | `Scene::DestroyEntity` accepts an entity handle without checking that it belongs to the scene or remains valid. | A stale handle or entity from another scene can target the wrong registry entity when handles coincide. Validate ownership and registry membership at API boundaries. `Scene.cpp`, `Entity.h`. |
| **Low** | Build requirements disagree: README says CMake 4.0, while the root CMake file requires 4.1.2. | New contributors can install the documented version and still fail configuration. `README.md`, root `CMakeLists.txt`. |

These are source-level findings, not a claim that every issue was reproduced at runtime. The assessment was performed on this checkout; Windows and macOS builds/runs were not performed here.

## What must be done before people can use it to make games

The smallest credible “usable for games” bar is a **complete and repeatable 2D workflow**:

- Create/open/save a scene without losing entities, components, texture assignments, or identity; reject bad files safely and do not destroy the current scene on a failed open.
- Add a play/stop mode that runs scene updates, initializes and destroys scripts deterministically, routes input to game code, and returns to the editor scene safely.
- Make the main loop, viewport resizing, picking, asset loading, and entity lifetime safe against zero dimensions, invalid paths, stale handles, and failed resource creation.
- Support a basic game content set: textured sprites, stable relative asset references, scene transitions, input mapping, and a small audio playback interface. Add physics/collision when the chosen first game requires it; do not block the first playable slice on a large general-purpose physics framework.
- Produce a runnable packaged build from a project, with assets copied or referenced predictably and no dependence on the repository checkout or current working directory.
- Add automated tests for scene round-trips and errors, entity/script lifetime, and core data conversions; add a CI build for the primary development platform.

Until these points are addressed, Nebula is appropriate for **learning, graphics experiments, and editor prototyping**, but not yet for relying on it to author and ship a game.

## Recommended next steps

### 1. Stabilize the 2D vertical slice

Make one small game (for example, a sprite-based arcade prototype) the acceptance target. Fix serialization and texture references first, add a play/stop runtime, and make scene/script lifetime explicit. Validate the target with automated scene round-trip and runtime smoke tests. This tests the entire product path and is more informative than adding isolated editor controls.

### 2. Establish a safety and test baseline

Add tests around entity creation/destruction, malformed/missing scenes, saved texture paths, viewport bounds, and model-import failures. Add one reproducible primary-platform CI build and document exact setup/run steps. Make errors visible in the editor rather than assertions or uncaught exceptions. Resolve the CMake documentation mismatch.

### 3. Complete the asset workflow

Introduce a project root and stable relative asset identifiers. Persist references, validate import results, and make imported assets inspectable. Add reimport/hot reload only after identity and failure handling are reliable. Avoid storing machine-specific absolute paths in scene files.

### 4. Add the game features the first target actually needs

Prioritize input actions, scene transitions, timing, audio, and a minimal collision/physics story based on the selected game. Decide whether native C++ scripts are the long-term gameplay interface or an initial prototype; in either case, define ownership, lifecycle, serialization, and runtime error behavior. Avoid building a broad scripting system before one game has exercised the interface.

### 5. Grow 3D and platform coverage deliberately

After the 2D workflow is dependable, integrate models into scenes with materials/textures and a scene renderer; add animation only against a concrete sample. Keep OpenGL and Metal parity on a tested feature matrix, and exercise Windows/macOS in CI or documented manual release checks. The code currently demonstrates those backends, but there is not enough project-level test evidence to treat parity as assured.

### 6. Improve editor productivity after the runtime path works

Add undo/redo, parent/child transforms, prefabs, asset previews, and project settings when the core save/run loop is stable. These features matter to daily use, but they cannot compensate for lossy scene data or the lack of a game runtime.

## Bottom line

Nebula has a solid learning-project foundation and enough editor/rendering work to justify continuing. The highest-value focus is **correct scene persistence, runtime/play mode, lifecycle safety, and tests**, not adding more rendering abstractions. Once a small 2D game can be authored, saved, played, and packaged repeatedly—and that workflow is covered by tests—the engine will have crossed the line from prototype to a tool people can reasonably use to make simple games.
