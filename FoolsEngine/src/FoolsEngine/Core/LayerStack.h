#pragma once

#include "FE_pch.h"
#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Core/Layer.h"

namespace fe
{
	class LayerStack
	{
	public:
		LayerStack() { FE_PROFILER_FUNC(); }
		~LayerStack();

		void PushInnerLayer(std::shared_ptr<Layer> layer);
		void PushOuterLayer(std::shared_ptr<Layer> layer);
		void PopInnerLayer(std::shared_ptr<Layer> layer);
		void PopOuterLayer(std::shared_ptr<Layer> layer);

		std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
		std::vector<std::shared_ptr<Layer>>::iterator end()   { return m_Layers.end(); }
		std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<std::shared_ptr<Layer>>::reverse_iterator rend()   { return m_Layers.rend(); }
	private:
		std::vector<std::shared_ptr<Layer>> m_Layers;
		unsigned int m_InnerLayerInsertIndex = 0;
	};
}

