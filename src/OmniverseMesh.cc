/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "OmniverseMesh.hh"

#include <exception>

namespace ignition::rendering::omni {
bool OmniverseMesh::HasSkeleton() const {
  throw std::runtime_error("not implemented");
}
std::map<std::string, math::Matrix4d> OmniverseMesh::SkeletonLocalTransforms()
    const {
  throw std::runtime_error("not implemented");
}
void OmniverseMesh::SetSkeletonLocalTransforms(
    const std::map<std::string, math::Matrix4d> &_tfs) {
  throw std::runtime_error("not implemented");
}
std::unordered_map<std::string, float> OmniverseMesh::SkeletonWeights() const {
  throw std::runtime_error("not implemented");
}
void OmniverseMesh::SetSkeletonWeights(
    const std::unordered_map<std::string, float> &_weights) {
  throw std::runtime_error("not implemented");
}
void OmniverseMesh::SetSkeletonAnimationEnabled(const std::string &_name,
                                                bool _enabled, bool _loop,
                                                float _weight) {
  throw std::runtime_error("not implemented");
}
bool OmniverseMesh::SkeletonAnimationEnabled(const std::string &_name) const {
  throw std::runtime_error("not implemented");
}
void OmniverseMesh::UpdateSkeletonAnimation(
    std::chrono::steady_clock::duration _time) {
  throw std::runtime_error("not implemented");
}
unsigned int OmniverseMesh::SubMeshCount() const {
  throw std::runtime_error("not implemented");
}
bool OmniverseMesh::HasSubMesh(ConstSubMeshPtr _subMesh) const {
  throw std::runtime_error("not implemented");
}
bool OmniverseMesh::HasSubMeshName(const std::string &_name) const {
  throw std::runtime_error("not implemented");
}
SubMeshPtr OmniverseMesh::SubMeshByName(const std::string &_name) const {
  throw std::runtime_error("not implemented");
}
SubMeshPtr OmniverseMesh::SubMeshByIndex(unsigned int _index) const {
  throw std::runtime_error("not implemented");
}
void OmniverseMesh::SetDescriptor(const MeshDescriptor &_desc) {
  throw std::runtime_error("not implemented");
}
const MeshDescriptor &OmniverseMesh::Descriptor() const {
  throw std::runtime_error("not implemented");
}

MaterialPtr OmniverseSubMesh::Material() const {
  throw std::runtime_error("not implemented");
}
void OmniverseSubMesh::SetMaterial(const std::string &_name, bool _unique) {
  throw std::runtime_error("not implemented");
}
void OmniverseSubMesh::SetMaterial(MaterialPtr _material, bool _unique) {
  throw std::runtime_error("not implemented");
}
}  // namespace ignition::rendering::omni
