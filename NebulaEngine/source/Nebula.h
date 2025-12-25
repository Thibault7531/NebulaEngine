#pragma once

// For use by Nebula Application

// Native application
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/CoreSystems/Application/Layer.h"
#include "Nebula/CoreSystems/Core/Log.h"
#include "Nebula/CoreSystems/Core/Timestep.h"

// Events system
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"

// Input system
#include "Nebula/Renderer/Cameras/CameraController.h"
#include "Platform/Nebula/CoreSystems/Input/Input.h"
#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Input/MouseButtonCodes.h"

// ImGui
#include "Nebula/ImGui/ImGuiLayer.h"

// Renderer
#include "Nebula/Renderer/Cameras/Camera.h"
#include "Nebula/Renderer/Buffers/Framebuffer.h"
#include "Nebula/Renderer/Cameras/OrthographicCamera.h"
#include "Nebula/Renderer/Cameras/PerspectiveCamera.h"
#include "Nebula/Renderer/Core/RenderCommand.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Renderer/Renderer2D.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Texture.h"

// Scene
#include "Nebula/Scene/Components.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/ScriptableEntity.h"
