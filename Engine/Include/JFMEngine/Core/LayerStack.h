//
// Created by kw on 25-7-24.
// 图层栈 - 管理多个图层的容器
//

#ifndef LAYERSTACK_H
#define LAYERSTACK_H

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Core/Layer.h"
#include <vector>
#include <memory>

namespace JFM {

    class JFM_API LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        // 图层管理
        void PushLayer(std::shared_ptr<Layer> layer);// 添加普通图层
        void PushOverlay(std::shared_ptr<Layer> overlay);// 添加覆盖层（通常是UI层）
        // 覆盖层通常在栈顶，普通图层在覆盖层之前
        void PopLayer(std::shared_ptr<Layer> layer);
        void PopOverlay(std::shared_ptr<Layer> overlay);

        // 迭代器支持
        std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::iterator end() { return m_Layers.end(); }
        std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }

        std::vector<std::shared_ptr<Layer>>::const_iterator begin() const { return m_Layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::const_iterator end() const { return m_Layers.end(); }
        std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }

        // 获取图层数量
        size_t Size() const { return m_Layers.size(); }

        // 清空所有图层
        void Clear();

        // 查找图层
        std::shared_ptr<Layer> FindLayer(const std::string& name);

    private:
        std::vector<std::shared_ptr<Layer>> m_Layers;
        unsigned int m_LayerInsertIndex = 0; // 普通图层插入位置
    };

}

#endif // LAYERSTACK_H
