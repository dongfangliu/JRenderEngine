//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_SCENE_H_
#define JRENDERENGINE_INCLUDE_SCENE_H_
#include "transform.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
using namespace  std;
class SceneObject{
 private:
  string name;
  Transform transform_;
  vector<SceneObject*> children_;
  SceneObject* parent_= nullptr;
  map<type_info,vector<void *>> components_;
 public:
  SceneObject() = default;
  ~SceneObject() = default;
  SceneObject(const string &name);
  void SetLocalTranslation(glm::vec3 translation);
  void SetLocalRotation(glm::vec3 rotation);
  void SetLocalScale(glm::vec3 scale);
  void SetParent(SceneObject* parent);
  void AddChild(SceneObject* child);
  [[nodiscard]] const glm::mat4 &GetWorldTransform() const;

  template<class T> T& GetComponent(){
    if(components_.find(typeid(T))!=components_.end() && !components_[typeid(T)].empty()){
      return static_pointer_cast<T&>(components_[typeid(T)][0]);
    }else{
      return nullptr;
    }
  }

  template<class T> vector<void *> GetComponents(){
    if(components_.find(typeid(T))!=components_.end() && !components_[typeid(T)].empty()){
      return components_[typeid(T)];
    }else{
      return {};
    }
  }
  template<class T> void AddComponent(T& component){
    if(components_.find(typeid(T))!=components_.end()){
      components_[typeid(T)].push_back(component);
    }else{
      vector<T&> v;v.push_back(component);
      components_[typeid(T)] = v;
    }
  }
};
class Scene {
  SceneObject root;
};

#endif //JRENDERENGINE_INCLUDE_SCENE_H_
