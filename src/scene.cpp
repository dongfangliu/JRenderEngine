//
// Created by Lenovo on 2022/4/26.
//

#include "scene.h"
const glm::mat4 &SceneObject::GetWorldTransform() const {
  if(this->parent_!= nullptr){
    return parent_->GetWorldTransform()*transform_.GetTransformMatrix();
  }else{
    return transform_.GetTransformMatrix();
  }
}
void SceneObject::SetLocalTranslation(glm::vec3 translation) {
  this->transform_.translation = translation;
}
void SceneObject::SetLocalRotation(glm::vec3 rotation) {
  this->transform_.rotation = rotation;
}
void SceneObject::SetParent(SceneObject *parent) {
  std::remove(parent_->children_.begin(), parent_->children_.end(), this);
  parent->AddChild(this);
}
void SceneObject::AddChild(SceneObject *child) {
  this->children_.push_back(child);
  child->parent_ = this;
}
SceneObject::SceneObject(const string &name) : name(name) {

}
void SceneObject::SetLocalScale(glm::vec3 scale) {
  this->transform_.scale = scale;
}

