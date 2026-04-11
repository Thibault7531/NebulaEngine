#include "RenderCommand.h"

#ifdef NB_PLATFORM_MACOS
	#include "Platform/OpenGL410/OpenGL410RendererAPI.h"
#else
	#include "Platform/OpenGL/OpenGLRendererAPI.h"
	#include "Platform/OpenGL410/OpenGL410RendererAPI.h"
#endif

namespace Nebula {

#ifdef NB_PLATFORM_MACOS
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGL410RendererAPI;
#else
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
#endif

}
