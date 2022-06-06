#pragma once 

#include <string>
#include "glm/glm.hpp"
#include "shader.h"
using namespace std;


struct PBRMaterial{
  PBRMaterial(const glm::vec4 &base_color_factor,
              float metallic_factor,
              float roughness_facotr,
              const string &base_color_texture,
              const string &normal_texture,
              const string &metallic_roughness_texture,
              int alphaMode,float alphaCutOff,bool doubleSided

              )
      : baseColorFactor(base_color_factor),
        metallicFactor(metallic_factor),
        roughnessFactor(roughness_facotr),
        baseColorTexture(base_color_texture),
        normalTexture(normal_texture),
        metallicRoughnessTexture(metallic_roughness_texture) ,
        alphaMode(alphaMode),alphaCutOff(alphaCutOff),doubleSided(doubleSided)
        {
    if(!base_color_texture.empty()){
      bcMode+= 1<<1;
    }
    if(baseColorFactor!=glm::vec4(0)){
      bcMode+=1<<2;
    }
    if(!metallic_roughness_texture.empty()){
      metallicMode+= 1<<1;roughnessMode+= 1<<1;
    }
    if(roughnessFactor!=0){
      roughnessMode+=1<<2;
    }
    if(metallicFactor!=0){
      metallicMode+=1<<2;
    }
  }
   glm::vec4 baseColorFactor;
	float metallicFactor=0;
	float roughnessFactor=0;
	string baseColorTexture;
	string normalTexture;
	string metallicRoughnessTexture;

    int bcMode=0,metallicMode=0,roughnessMode=0;
  bool doubleSided = false;
  float alphaCutOff = 0.5;
  int  alphaMode = 0;
};