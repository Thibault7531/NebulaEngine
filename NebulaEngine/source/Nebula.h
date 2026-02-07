#pragma once

// For use by Nebula Application

// Native application
#include "Core/Nebula/CoreSystems/Application/Application.h"
#include "Core/Nebula/CoreSystems/Application/Layer.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "Core/Nebula/CoreSystems/Core/Timestep.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

// Events system
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"

// Input system
#include "Core/Nebula/Renderer/Cameras/CameraController.h"
#include "Platform/Nebula/CoreSystems/Input/Input.h"
#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Input/MouseButtonCodes.h"

// ImGui
#include "Core/Nebula/ImGui/ImGuiLayer.h"

// Renderer
#include "Core/Nebula/Renderer/Renderer.h"
#include "Core/Nebula/Renderer/Renderer2D.h"
#include "Core/Nebula/Renderer/Shader.h"
#include "Core/Nebula/Renderer/Texture.h"
#include "Core/Nebula/Renderer/Buffers/Framebuffer.h"
#include "Core/Nebula/Renderer/Buffers/VertexArray.h"
#include "Core/Nebula/Renderer/Cameras/Camera.h"
#include "Core/Nebula/Renderer/Cameras/OrthographicCamera.h"
#include "Core/Nebula/Renderer/Cameras/PerspectiveCamera.h"
#include "Core/Nebula/Renderer/Core/RenderCommand.h"

// Scene
#include "Core/Nebula/Scene/Components.h"
#include "Core/Nebula/Scene/Entity.h"
#include "Core/Nebula/Scene/Scene.h"
#include "Core/Nebula/Scene/ScriptableEntity.h"
