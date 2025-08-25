//
// PostProcessing.h - 后处理效果系统
// 支持HDR、Bloom、色调映射等后处理效果
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include <memory>

namespace JFM {

    class JFM_API Framebuffer {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const = 0;
        virtual uint32_t GetDepthAttachmentID() const = 0;

        static std::shared_ptr<Framebuffer> Create(uint32_t width, uint32_t height, bool hdr = false);
    };

    class JFM_API PostProcessingStack {
    public:
        PostProcessingStack(uint32_t width, uint32_t height);
        ~PostProcessingStack() = default;

        void BeginRender();
        void EndRender();

        void Resize(uint32_t width, uint32_t height);
        void RenderToScreen();

        // 后处理效果开关
        void SetHDR(bool enabled) { m_HDREnabled = enabled; }
        void SetBloom(bool enabled) { m_BloomEnabled = enabled; }
        void SetGammaCorrection(bool enabled) { m_GammaCorrectionEnabled = enabled; }
        void SetToneMapping(bool enabled) { m_ToneMappingEnabled = enabled; }

        // 参数调节
        void SetExposure(float exposure) { m_Exposure = exposure; }
        void SetGamma(float gamma) { m_Gamma = gamma; }
        void SetBloomThreshold(float threshold) { m_BloomThreshold = threshold; }

    private:
        std::shared_ptr<Framebuffer> m_HDRFramebuffer;
        std::shared_ptr<Framebuffer> m_BloomFramebuffers[2];
        std::shared_ptr<Shader> m_HDRShader;
        std::shared_ptr<Shader> m_BloomShader;
        std::shared_ptr<Shader> m_BlurShader;
        std::shared_ptr<VertexArray> m_QuadVAO;

        uint32_t m_Width, m_Height;
        bool m_HDREnabled = true;
        bool m_BloomEnabled = true;
        bool m_GammaCorrectionEnabled = true;
        bool m_ToneMappingEnabled = true;

        float m_Exposure = 1.0f;
        float m_Gamma = 2.2f;
        float m_BloomThreshold = 1.0f;

        void CreateQuad();
        void ApplyBloom();
    };

}
