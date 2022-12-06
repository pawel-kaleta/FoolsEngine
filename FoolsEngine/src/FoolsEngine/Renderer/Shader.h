#pragma once

namespace fe
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Shader* Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	private:

	};

}
