#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Buffers.h"
#include "Texture.h"

namespace fe
{
	class Uniform
	{
	public:
		Uniform(const std::string& name, ShaderData::Type type)
			: m_Name(name), m_Type(type) {}

		Uniform()
			: m_Name(""), m_Type(ShaderData::Type::None) {}

		inline const std::string& GetName() const { return m_Name; }
		inline const ShaderData::Type& GetType() const { return m_Type; }
		inline const ShaderData::Structure GetStructure() const { return ShaderData::StructureInType(m_Type); }
		inline const ShaderData::Primitive GetPrimitive() const { return ShaderData::PrimitiveInType(m_Type); }
		inline const uint32_t GetSize() const { return SizeOfType(m_Type); }
	private:
		std::string m_Name;
		ShaderData::Type m_Type;
	};

	class ShaderTextureSlot
	{
	public:
		ShaderTextureSlot(const std::string& name, TextureType type)
			: m_Name(name), m_Type(type) {}

		inline const TextureType GetType() const { return m_Type; }
		inline const std::string& GetName() const { return m_Name; }
	private:
		TextureType m_Type;
		std::string m_Name;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) = 0;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t rendererTextureSlot) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Scope<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, GDIType GDI);
		static Scope<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		static Scope<Shader> Create(const std::string& name, const std::string& shaderSource, GDIType GDI);
		static Scope<Shader> Create(const std::string& name, const std::string& shaderSource);
		static Scope<Shader> Create(const std::string& filePath, GDIType GDI);
		static Scope<Shader> Create(const std::string& filePath);
	private:

	};

	class ShaderLibrary
	{
	public:
		inline static void Add(const Ref<Shader> shader) { s_ActiveInstance->IAdd(shader); }
		inline static Ref<Shader> Load(const std::string& filePath) { return s_ActiveInstance->ILoad(filePath); }

		inline static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		inline static Ref<Shader> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		inline static void SetActiveInstance(ShaderLibrary* shaderLib) { s_ActiveInstance = shaderLib; }

		void IAdd(const Ref<Shader> shader);
		Ref<Shader> ILoad(const std::string& filePath);

		bool IExist(const std::string& name) const;
		Ref<Shader> IGet(const std::string& name);
	
	private:
		static ShaderLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
