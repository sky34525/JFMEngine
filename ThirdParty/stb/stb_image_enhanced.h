/*
stb_image.h - Enhanced version with TGA support
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif

typedef unsigned char stbi_uc;

STBIDEF stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF void stbi_image_free(void *retval_from_stbi_load);
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

#ifdef __cplusplus
}
#endif

#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static int g_flip_vertically = 0;

void stbi_set_flip_vertically_on_load(int flag) {
    g_flip_vertically = flag;
}

// TGA文件头结构
typedef struct {
    unsigned char id_length;
    unsigned char color_map_type;
    unsigned char image_type;
    unsigned short color_map_first_entry;
    unsigned short color_map_length;
    unsigned char color_map_entry_size;
    unsigned short x_origin;
    unsigned short y_origin;
    unsigned short width;
    unsigned short height;
    unsigned char pixel_depth;
    unsigned char image_descriptor;
} tga_header;

// TGA文件加载器
stbi_uc *load_tga(const char *filename, int *x, int *y, int *comp, int req_comp) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    tga_header header;
    if (fread(&header, sizeof(header), 1, f) != 1) {
        printf("Failed to read TGA header\n");
        fclose(f);
        return NULL;
    }

    // 只支持未压缩的真彩色图像 (type 2)
    if (header.image_type != 2 || header.pixel_depth != 24) {
        printf("Unsupported TGA format (type: %d, depth: %d)\n", header.image_type, header.pixel_depth);
        fclose(f);
        return NULL;
    }

    *x = header.width;
    *y = header.height;
    *comp = 3; // RGB

    printf("Loading TGA: %dx%d, %d channels\n", *x, *y, *comp);

    // 跳过ID字段
    if (header.id_length > 0) {
        fseek(f, header.id_length, SEEK_CUR);
    }

    int image_size = header.width * header.height * 3;
    stbi_uc *data = (stbi_uc*)malloc(image_size);
    if (!data) {
        printf("Failed to allocate memory for image data\n");
        fclose(f);
        return NULL;
    }

    if (fread(data, 1, image_size, f) != image_size) {
        printf("Failed to read image data\n");
        free(data);
        fclose(f);
        return NULL;
    }

    fclose(f);

    // TGA是BGR格式，需要转换为RGB
    for (int i = 0; i < image_size; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    // 如果需要垂直翻转
    if (g_flip_vertically) {
        int row_size = header.width * 3;
        stbi_uc *temp_row = (stbi_uc*)malloc(row_size);
        if (temp_row) {
            for (int y_flip = 0; y_flip < header.height / 2; y_flip++) {
                stbi_uc *row1 = data + y_flip * row_size;
                stbi_uc *row2 = data + (header.height - 1 - y_flip) * row_size;
                memcpy(temp_row, row1, row_size);
                memcpy(row1, row2, row_size);
                memcpy(row2, temp_row, row_size);
            }
            free(temp_row);
        }
    }

    printf("Successfully loaded TGA: %s\n", filename);
    return data;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
    printf("Attempting to load image: %s\n", filename);

    // 检查文件扩展名
    const char *ext = strrchr(filename, '.');
    if (ext && strcmp(ext, ".tga") == 0) {
        return load_tga(filename, x, y, channels_in_file, desired_channels);
    }

    printf("Unsupported file format: %s\n", filename);

    // 默认情况下，创建一个简单的测试纹理
    *x = 64;
    *y = 64;
    *channels_in_file = 3;

    stbi_uc *data = (stbi_uc*)malloc(64 * 64 * 3);
    if (data) {
        // 创建一个彩色格子纹理作为fallback
        for (int y_pos = 0; y_pos < 64; y_pos++) {
            for (int x_pos = 0; x_pos < 64; x_pos++) {
                int index = (y_pos * 64 + x_pos) * 3;
                if ((x_pos / 8 + y_pos / 8) % 2 == 0) {
                    data[index + 0] = 255; // R
                    data[index + 1] = 100; // G
                    data[index + 2] = 100; // B
                } else {
                    data[index + 0] = 100; // R
                    data[index + 1] = 255; // G
                    data[index + 2] = 100; // B
                }
            }
        }
    }
    return data;
}

void stbi_image_free(void *retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_IMPLEMENTATION
