/*
stb_image.h - v2.28 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

Do this:
   #define STB_IMAGE_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using malloc,realloc,free
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image from memory or from file
// return image, or NULL on failure
// x, y = width, height; comp = # of components (1=Y, 2=YA, 3=RGB, 4=RGBA)
//

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

STBIDEF stbi_uc *stbi_load_from_memory   (stbi_uc           const *buffer, int len   , int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load               (char              const *filename, int *x, int *y, int *channels_in_file, int desired_channels);

STBIDEF void     stbi_image_free      (void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int      stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp);
STBIDEF int      stbi_info            (char const *filename,     int *x, int *y, int *comp);

// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// as above, but only applies to images loaded on the thread that calls this function
STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

// Simple implementation for demonstration purposes
// This is a minimal stub - in production, you'd use the full stb_image.h

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

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
    // For demo purposes, create a simple 2x2 white texture
    *x = 2;
    *y = 2;
    *channels_in_file = 4;

    stbi_uc *data = (stbi_uc*)malloc(2 * 2 * 4);
    if (data) {
        // Create a simple white texture
        for (int i = 0; i < 16; i++) {
            data[i] = 255;
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
