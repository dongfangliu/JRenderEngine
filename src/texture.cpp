//
// Created by Lenovo on 2022/4/26.
//

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
Texture::Texture(const aiTextureType&type,
                 const string &path)
    : type(type), path(path) {

    unsigned char * load_data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (load_data) {
        data.resize(width * height * nrComponents);
        memcpy(data.data(), load_data, sizeof(unsigned char) * width * height * nrComponents);

        stbi_image_free(load_data);
    }
  if (!data.empty())
  {
    format = 0;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
}
void Texture::SetupGL()  {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture() {}
Texture::~Texture() {
}
