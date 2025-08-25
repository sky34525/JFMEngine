#include "JFMEngine/TextureLoader.h"
#include "stb_image.h"
#include <iostream>
#include <stdexcept>

namespace JFMEngine {

// ImageData 析构函数
ImageData::~ImageData() {
    if (data) {
        stbi_image_free(data);
        data = nullptr;
    }
}

// ImageData 移动构造函数
ImageData::ImageData(ImageData&& other) noexcept
    : data(other.data), width(other.width), height(other.height), channels(other.channels) {
    other.data = nullptr;
    other.width = 0;
    other.height = 0;
    other.channels = 0;
}

// ImageData 移动赋值操作符
ImageData& ImageData::operator=(ImageData&& other) noexcept {
    if (this != &other) {
        if (data) {
            stbi_image_free(data);
        }

        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;

        other.data = nullptr;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
    }
    return *this;
}

std::unique_ptr<ImageData> TextureLoader::LoadFromFile(const std::string& filepath, int desired_channels) {
    auto imageData = std::make_unique<ImageData>();

    imageData->data = stbi_load(filepath.c_str(), &imageData->width, &imageData->height, &imageData->channels, desired_channels);

    if (!imageData->data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return nullptr;
    }

    // 如果指定了desired_channels，更新channels值
    if (desired_channels > 0) {
        imageData->channels = desired_channels;
    }

    std::cout << "Successfully loaded image: " << filepath
              << " (" << imageData->width << "x" << imageData->height
              << ", " << imageData->channels << " channels)" << std::endl;

    return imageData;
}

std::unique_ptr<ImageData> TextureLoader::LoadFromMemory(const unsigned char* buffer, int len, int desired_channels) {
    auto imageData = std::make_unique<ImageData>();

    imageData->data = stbi_load_from_memory(buffer, len, &imageData->width, &imageData->height, &imageData->channels, desired_channels);

    if (!imageData->data) {
        std::cerr << "Failed to load image from memory" << std::endl;
        return nullptr;
    }

    // 如果指定了desired_channels，更新channels值
    if (desired_channels > 0) {
        imageData->channels = desired_channels;
    }

    std::cout << "Successfully loaded image from memory"
              << " (" << imageData->width << "x" << imageData->height
              << ", " << imageData->channels << " channels)" << std::endl;

    return imageData;
}

void TextureLoader::SetFlipVerticallyOnLoad(bool flip) {
    stbi_set_flip_vertically_on_load(flip ? 1 : 0);
}

void TextureLoader::FreeImageData(unsigned char* data) {
    if (data) {
        stbi_image_free(data);
    }
}

} // namespace JFMEngine
