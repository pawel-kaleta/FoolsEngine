#include "FE_pch.h"
#include "LayerStack.h"

namespace fe
{
	LayerStack::~LayerStack()
	{
		FE_PROFILER_FUNC();

		for (auto layer_it = m_Layers.begin(); layer_it != m_Layers.end(); layer_it++)
		{
			(*layer_it)->OnDetach();
		}
		m_Layers.clear();
	}

	void LayerStack::PushInnerLayer(Ref<Layer> layer)
	{
		FE_PROFILER_FUNC();

		m_Layers.insert(m_Layers.begin() + m_InnerLayerInsertIndex, layer);
		m_InnerLayerInsertIndex++;

		FE_LOG_CORE_INFO("New inner layer \"{0}\" added to LayerStack", layer->ToString());

		layer->OnAttach();
	}

	void LayerStack::PushOuterLayer(Ref<Layer> layer)
	{
		FE_PROFILER_FUNC();

		m_Layers.push_back(layer);

		FE_LOG_CORE_INFO("New outer layer \"{0}\" added to LayerStack", layer->ToString());

		layer->OnAttach();
	}

	void LayerStack::PopInnerLayer(Ref<Layer> layer)
	{
		FE_PROFILER_FUNC();

		auto layer_it = std::find(m_Layers.begin(), m_Layers.begin() + m_InnerLayerInsertIndex, layer);
		if (layer_it != m_Layers.begin() + m_InnerLayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(layer_it);
			m_InnerLayerInsertIndex--;

			FE_LOG_CORE_INFO("Inner layer \"{0}\" removed from LayerStack", layer->ToString());
		}
		else
		{
			FE_LOG_CORE_WARN("Attempt to remove inner layer \"{0}\" from the LayerStack without pushing it first!", layer->ToString());
		}
	}

	void LayerStack::PopOuterLayer(Ref<Layer> layer)
	{
		FE_PROFILER_FUNC();

		auto layer_it = std::find(m_Layers.begin() + m_InnerLayerInsertIndex, m_Layers.end(), layer);
		if (layer_it != m_Layers.end())
		{
			layer->OnDetach();
			m_Layers.erase(layer_it);

			FE_LOG_CORE_INFO("Outer layer \"{0}\" removed from LayerStack", layer->ToString());
		}
		else
		{
			FE_LOG_CORE_WARN("Attempt to remove outer layer \"{0}\" from the LayerStack without pushing it first!", layer->ToString());
		}
	}
}