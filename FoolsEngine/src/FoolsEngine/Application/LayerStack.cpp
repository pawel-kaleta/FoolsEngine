#include "LayerStack.h"

#include "FoolsEngine/Foundation/Utils/Core.h"

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

		Context::Log::Core.Info("New inner layer \"{0}\" added to LayerStack"_FE, layer->ToString());

		layer->OnAttach();
	}

	void LayerStack::PushOuterLayer(Ref<Layer> layer)
	{
		FE_PROFILER_FUNC();

		m_Layers.push_back(layer);

		Context::Log::Core.Info("New outer layer {0} added to LayerStack"_FE, layer->ToString());

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

			Context::Log::Core.Info("Inner layer \"{0}\" removed from LayerStack"_FE, layer->ToString());
		}
		else
		{
			Context::Log::Core.Warn("Attempt to remove inner layer \"{0}\" from the LayerStack without pushing it first!"_FE, layer->ToString());
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

			Context::Log::Core.Info("Outer layer \"{0}\" removed from LayerStack"_FE, layer->ToString());
		}
		else
		{
			Context::Log::Core.Warn("Attempt to remove outer layer \"{0}\" from the LayerStack without pushing it first!"_FE, layer->ToString());
		}
	}
}