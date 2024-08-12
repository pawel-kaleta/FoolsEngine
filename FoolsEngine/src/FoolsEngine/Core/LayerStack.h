#pragma once

#include "FoolsEngine/Core/Layer.h"

namespace fe
{
	class LayerStack
	{
	public:
		LayerStack() { FE_PROFILER_FUNC(); }
		~LayerStack();

		void PushInnerLayer(Ref<Layer> layer);
		void PushOuterLayer(Ref<Layer> layer);
		void PopInnerLayer(Ref<Layer> layer);
		void PopOuterLayer(Ref<Layer> layer);

		std::vector< Ref<Layer> >::iterator begin() { return m_Layers.begin(); }
		std::vector< Ref<Layer> >::iterator end()   { return m_Layers.end(); }
		std::vector< Ref<Layer> >::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector< Ref<Layer> >::reverse_iterator rend()   { return m_Layers.rend(); }
	private:
		friend class Application;
		std::vector< Ref<Layer> > m_Layers;
		unsigned int m_InnerLayerInsertIndex = 0;
	};
}

