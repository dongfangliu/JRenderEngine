//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_SCENE_H_
#define JRENDERENGINE_INCLUDE_SCENE_H_
#include "transform.h"
#include <memory>
#include <vector>
#include <string>
using namespace  std;
class SceneObject{
 private:
  string name;
  Transform transform_;
  vector<SceneObject*> children_;
  SceneObject* parent_= nullptr;
 public:
  SceneObject() = default;
  ~SceneObject() = default;
  SceneObject(const string &name);
  void SetLocalTranslation(glm::vec3 translation);
  void SetLocalRotation(glm::vec3 rotation);
  void SetLocalScale(glm::vec3 scale){
    this->transform_.scale = scale;
  }
  void SetParent(SceneObject* parent);
  void AddChild(SceneObject* child);
  [[nodiscard]] const glm::mat4 &GetWorldTransform() const;

};
class Scene {
  SceneObject root;
};

#endif //JRENDERENGINE_INCLUDE_SCENE_H_
