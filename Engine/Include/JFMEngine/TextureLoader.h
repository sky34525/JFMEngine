#pragma once

#include <string>
#include <memory>

namespace JFMEngine {

struct ImageData {
    unsigned char* data;
    int width;
    int height;
    int channels;

    ImageData() : data(nullptr), width(0), height(0), channels(0) {}
    ~ImageData();

    // 禁用拷贝构造和赋值
    ImageData(const ImageData&) = delete;
    ImageData& operator=(const ImageData&) = delete;

    // 允许移动构造和赋值
    ImageData(ImageData&& other) noexcept;
    ImageData& operator=(ImageData&& other) noexcept;
};

class TextureLoader {
public:
    // 从文件加载图像
    static std::unique_ptr<ImageData> LoadFromFile(const std::string& filepath, int desired_channels = 0);

    // 从内存加载图像
    static std::unique_ptr<ImageData> LoadFromMemory(const unsigned char* buffer, int len, int desired_channels = 0);

    // 设置是否垂直翻转图像（对OpenGL很有用）
    static void SetFlipVerticallyOnLoad(bool flip);

    // 释放图像数据
    static void FreeImageData(unsigned char* data);

private:
    TextureLoader() = default;
};

} // namespace JFMEngine
