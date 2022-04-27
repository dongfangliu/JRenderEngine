//
// Created by Lenovo on 2022/4/26.
//

#include "transform.h"
Transform::Transform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
    : translation(translation), rotation(rotation), scale(scale) {

}
glm::mat4 Transform::GetTransformMatrix() const {
  return glm::translate(glm::mat4(),translation)*glm::orientate4(rotation)*glm::scale(scale);
}
