//
// Created by Lenovo on 2022/4/26.
//

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <filesystem>
Texture::Texture(
                 const string &path, bool sRGB)
    : path(path), sRGB(sRGB) {

  HDR = std::filesystem::path(path).extension() == ".hdr";
  void *load_data;
  if (!HDR) {
    load_data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
  } else {
    load_data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
  }

  if (load_data) {
    if (!HDR) {
      data = malloc(width * height * nrComponents * sizeof(unsigned char));
      memcpy(data, load_data, sizeof(unsigned char) * width * height * nrComponents);
    } else {
      data = malloc(width * height * nrComponents * sizeof(float));
      memcpy(data, load_data, sizeof(float) * width * height * nrComponents);
    }
    stbi_image_free(load_data);
  }
  if (data != nullptr) {
    if (nrComponents == 1) {
      format = GL_RED;
      internalFormat = GL_RED;
    } else if (nrComponents == 3) {
      format = GL_RGB;
      if (sRGB) {
        internalFormat = GL_SRGB;
      } else {
        internalFormat = GL_RGB;
      }
    } else if (nrComponents == 4) {
      format = GL_RGBA;
      if (sRGB) {
        internalFormat = GL_SRGB_ALPHA;
      } else {
        internalFormat = GL_RGBA;
      }
    }
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
}
void Texture::SetupGL() {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  if (!HDR) {
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
}

Texture::Texture() {}
Texture::~Texture() {
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
}
