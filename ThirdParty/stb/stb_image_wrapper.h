//
// stb_image.h wrapper - 图像加载库
// 单头文件库，用于加载PNG、JPG、TGA等格式
//

#pragma once

// 在实现文件中定义STB_IMAGE_IMPLEMENTATION
#ifdef STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_ONLY_BMP
#endif

#include <stb_image.h>
