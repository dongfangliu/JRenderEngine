//
// Created by Lenovo on 2022/6/6.
//

#ifndef JRENDERENGINE_INCLUDE_CUBEMAP_H_
#define JRENDERENGINE_INCLUDE_CUBEMAP_H_
#include "texture.h"
#include "mesh.h"


class CubeMap {
 public:
  unsigned  int id;
  unsigned int mapResolution;
  vector<shared_ptr<Texture>> cubeTextures;
  void GenerateFromHDRTex(const string& hdrFilePath);

  void DiffuseIrradianceCalFrom(shared_ptr<CubeMap> envMap);
  void PrefilterEnvMap(shared_ptr<CubeMap> envMap);
  shared_ptr<Texture> CalBRDFLUT(shared_ptr<CubeMap> envMap);
  void SetupGL(bool useMip = false);
  explicit CubeMap(unsigned int map_resolution);
};

#endif //JRENDERENGINE_INCLUDE_CUBEMAP_H_
