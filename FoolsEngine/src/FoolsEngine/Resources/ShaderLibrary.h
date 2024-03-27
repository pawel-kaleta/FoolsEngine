#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

namespace fe
{
	class ShaderLibrary
	{
	public:
		static void Add(const Ref<Shader> shader) { s_ActiveInstance->IAdd(shader); }
		static Ref<Shader> Load(const std::string& filePath) { return s_ActiveInstance->ILoad(filePath); }

		static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		static Ref<Shader> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		static void SetActiveInstance(ShaderLibrary* shaderLib) { s_ActiveInstance = shaderLib; }
	private:
		friend class Renderer;

		static ShaderLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;

		void IAdd(const Ref<Shader> shader);
		Ref<Shader> ILoad(const std::string& filePath);

		bool IExist(const std::string& name) const;
		Ref<Shader> IGet(const std::string& name);
	};
}