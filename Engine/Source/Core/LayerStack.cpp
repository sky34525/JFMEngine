//
// Created by kw on 25-7-24.
// 图层栈实现
//

#include "JFMEngine/Core/LayerStack.h"
#include "JFMEngine/Utils/Log.h"
#include <algorithm>

namespace JFM {

    LayerStack::~LayerStack()
    {
        Clear();
    }

    void LayerStack::PushLayer(std::shared_ptr<Layer> layer)
    {
        // 普通图层插入到前半部分（在覆盖层之前）
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();

    }

    void LayerStack::PushOverlay(std::shared_ptr<Layer> overlay)
    {
        // 覆盖层始终在最后（最高优先级）
        m_Layers.emplace_back(overlay);
        overlay->OnAttach();

    }

    void LayerStack::PopLayer(std::shared_ptr<Layer> layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;

        }
    }

    void LayerStack::PopOverlay(std::shared_ptr<Layer> overlay)
    {
        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);

        }
    }

    void LayerStack::Clear()
    {
        for (auto& layer : m_Layers)
        {
            if (layer)
            {
                layer->OnDetach();
            }
        }
        m_Layers.clear();
        m_LayerInsertIndex = 0;

    }

    std::shared_ptr<Layer> LayerStack::FindLayer(const std::string& name)
    {
        for (auto& layer : m_Layers)
        {
            if (layer && layer->GetName() == name)
            {
                return layer;
            }
        }
        return nullptr;
    }

}
