#pragma once

#include <filesystem>
#include <slang-com-helper.h>
#include <slang-com-ptr.h>
#include <string>
#include <glad/gl.h>

#include "Nebula/Graphics/Shader.h"
#include "Nebula/Graphics/Buffers/Buffer.h"

namespace Nebula::Graphics {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& filepath);
		virtual ~OpenGLShader();

		[[nodiscard]] virtual const std::string& GetName() const override { return m_Name; }

	public:
		uint32_t GetSlangUniformLocation(const std::string& name) const;
		uint32_t GetUniformBlockLocation(const std::string& name);
		GLuint GetShaderID() const { return m_RendererID; }

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void CompileGLSL(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileSlang(const std::string& shader);
		void GenerateLayout();

	private:
		uint32_t m_RendererID;
		std::string m_Name;

		bool m_IsSlangShader = false;
		Slang::ComPtr<slang::ICompileRequest> m_CompileRequest;
		Slang::ComPtr<slang::IComponentType> m_ProgramReflection;
		ShaderReflectionLayout m_Layout;

	private:
		static bool s_IsSlangGlobalSessionCreated;
		static Slang::ComPtr<slang::IGlobalSession> s_GlobalSession;
	};

}
