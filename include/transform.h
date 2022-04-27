//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_TRANSFORM_H_
#define JRENDERENGINE_INCLUDE_TRANSFORM_H_
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/euler_angles.hpp"
class Transform {
 public:
  glm::vec3 translation{};
  //represent in euler angles;
  glm::vec3 rotation{};
  glm::vec3  scale{};
  ~Transform()=default;
  explicit Transform(const glm::vec3 &translation=glm::vec3(0, 0, 0), const glm::vec3 &rotation=glm::vec3(0, 0, 0), const glm::vec3 &scale=glm::vec3(1, 1, 1));
  [[nodiscard]] glm::mat4 GetTransformMatrix() const;
};

#endif //JRENDERENGINE_INCLUDE_TRANSFORM_H_

