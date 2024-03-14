#include <Texture.hpp>
#include <gragzo.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace gra;


template<>
Texture<unsigned char>::Texture(const char* filepath){
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    stbi_image_free(data);
};


// template<>
// Texture<unsigned char>::Texture(){
// };


template<>
Texture<unsigned char>::Texture(glm::vec4 col){
    data = (unsigned char*)malloc(100*100*3*sizeof(unsigned char));
    width = 100;
    height = 100;
    for(int i = 0; i < 100; i++){
        for(int q = 0; q < 100; q++){
            data[i*width*3+q*3] = (unsigned char)(col.r*255.0);
            data[i*width*3+q*3+1] = (unsigned char)(col.g*255.0);
            data[i*width*3+q*3+2] = (unsigned char)(col.b*255.0);
            data[i*width*3+q*3+3] = (unsigned char)(col.a*255.0);
        }
    }

    // for(int i = 0; i < 100*100*3; i++){
    //     data[i] = (unsigned char)(col.r*255.0);
    //     std::cout << (unsigned char)(col.r*255.0);
    // }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    free(data);

    // stbi_image_free(data);
};