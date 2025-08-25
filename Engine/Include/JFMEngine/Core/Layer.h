//
// Created by kw on 25-7-24.
// 图层基类 - 提供图层的基本接口
//

#ifndef LAYER_H
#define LAYER_H

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Events/Event.h"
#include <string>

namespace JFM {

    class JFM_API Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        // 图层生命周期管理
        virtual void OnAttach() {}//初始化图层资源
        virtual void OnDetach() {}//清理资源
        virtual void OnUpdate(float deltaTime) {}//逻辑更新
        virtual void OnRender() {}//渲染图层内容
        virtual void OnImGuiRender() {}// ImGui渲染
        virtual void OnEvent(JFM::Event& event) {}

        // 获取图层名称
        const std::string& GetName() const { return m_DebugName; }

        // 图层启用/禁用状态
        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }

    protected:
        std::string m_DebugName;// 图层调试名称
        bool m_Enabled = true;
    };

}

#endif // LAYER_H
