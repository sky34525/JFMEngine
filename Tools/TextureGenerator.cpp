//
// TextureGenerator.cpp - 纹理生成工具
// 用于创建测试用的纹理文件
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

// 简单的TGA图像格式写入
void WriteTGA(const std::string& filename, int width, int height, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);

    // TGA头部
    uint8_t header[18] = {0};
    header[2] = 2; // 未压缩的真彩色图像
    header[12] = width & 0xFF;
    header[13] = (width >> 8) & 0xFF;
    header[14] = height & 0xFF;
    header[15] = (height >> 8) & 0xFF;
    header[16] = 24; // 24位颜色深度

    file.write(reinterpret_cast<const char*>(header), 18);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}

// 生成砖墙纹理
void GenerateBrickTexture(const std::string& filename) {
    const int width = 256, height = 256;
    std::vector<uint8_t> data(width * height * 3);

    const int brickWidth = 32, brickHeight = 16;
    const int mortarWidth = 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;

            // 计算砖块位置
            int brickRow = y / (brickHeight + mortarWidth);
            int brickCol = x / (brickWidth + mortarWidth);

            // 交错砖块布局
            if (brickRow % 2 == 1) {
                brickCol = (x + brickWidth/2) / (brickWidth + mortarWidth);
            }

            int localX = x % (brickWidth + mortarWidth);
            int localY = y % (brickHeight + mortarWidth);

            if (localX < brickWidth && localY < brickHeight) {
                // 砖块颜色 - 红褐色
                data[index + 0] = 120 + (rand() % 40); // R
                data[index + 1] = 60 + (rand() % 30);  // G
                data[index + 2] = 40 + (rand() % 20);  // B
            } else {
                // 灰浆颜色 - 灰白色
                data[index + 0] = 200 + (rand() % 30); // R
                data[index + 1] = 200 + (rand() % 30); // G
                data[index + 2] = 180 + (rand() % 30); // B
            }
        }
    }

    WriteTGA(filename, width, height, data);
    std::cout << "Generated brick texture: " << filename << std::endl;
}

// 生成草地纹理
void GenerateGrassTexture(const std::string& filename) {
    const int width = 256, height = 256;
    std::vector<uint8_t> data(width * height * 3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;

            // 基础绿色
            uint8_t baseGreen = 80 + (rand() % 60);
            uint8_t baseRed = 20 + (rand() % 40);
            uint8_t baseBlue = 10 + (rand() % 30);

            // 添加一些草叶纹理
            if (rand() % 100 < 20) {
                baseGreen += 40;
                baseRed += 10;
            }

            data[index + 0] = baseRed;   // R
            data[index + 1] = baseGreen; // G
            data[index + 2] = baseBlue;  // B
        }
    }

    WriteTGA(filename, width, height, data);
    std::cout << "Generated grass texture: " << filename << std::endl;
}

// 生成木材纹理
void GenerateWoodTexture(const std::string& filename) {
    const int width = 256, height = 256;
    std::vector<uint8_t> data(width * height * 3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;

            // 木纹效果
            float woodPattern = sin(x * 0.1f) * 0.3f + sin(x * 0.05f) * 0.7f;
            woodPattern = (woodPattern + 1.0f) * 0.5f; // 归一化到0-1

            uint8_t woodColor = 100 + (uint8_t)(woodPattern * 80);
            uint8_t variation = rand() % 20;

            data[index + 0] = woodColor + variation;        // R
            data[index + 1] = woodColor * 0.7f + variation; // G
            data[index + 2] = woodColor * 0.4f + variation; // B
        }
    }

    WriteTGA(filename, width, height, data);
    std::cout << "Generated wood texture: " << filename << std::endl;
}

int main() {
    srand(42); // 固定随机种子，确保一致的结果

    GenerateBrickTexture("brick.tga");
    GenerateGrassTexture("grass.tga");
    GenerateWoodTexture("wood.tga");

    std::cout << "All textures generated successfully!" << std::endl;
    return 0;
}
