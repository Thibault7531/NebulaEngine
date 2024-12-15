#pragma once

#include <string>

#include "Nebula/Renderer/Shader.h"

namespace Nebula {
	/**
	 * OpenGL Shader.
	 */
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t m_RendererID;
	};

}
