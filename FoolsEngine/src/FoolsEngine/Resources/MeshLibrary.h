#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

namespace fe
{
	class MeshLibrary
	{
	public:
		static void Add(Ref<Mesh> mesh) { s_ActiveInstance->IAdd(mesh); }

		static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		static Ref<Mesh> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		static void SetActiveInstance(MeshLibrary* meshLib) { s_ActiveInstance = meshLib; }

		static const std::unordered_map<std::string, Ref<Mesh>>& GetAll() { return s_ActiveInstance->IGetAll(); }
	private:
		friend class Renderer;

		static MeshLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
		
		void IAdd(const Ref<Mesh> mesh);

		bool IExist(const std::string& name) const;
		Ref<Mesh> IGet(const std::string& name);
		const std::unordered_map<std::string, Ref<Mesh>>& IGetAll() { return m_Meshes; }
	};
}