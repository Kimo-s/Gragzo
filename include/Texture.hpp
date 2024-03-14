#pragma once

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
#include <glm/vec4.hpp>
#include <glm/common.hpp>

namespace gra{

    template<typename T>
    struct Pixel
    {
        T r, g, b, a;
    };

    template<typename T>
    class Texture {
    private:


    int width, height, nrChannels;
    T* data;


    public:

    unsigned int texture;

    Texture(const char* filepath);
    Texture(glm::vec4 col);

    };


}
