/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); * you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
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

#ifndef IGNITION_RENDERING_OMNI_OMNIVERSEMESH_HH
#define IGNITION_RENDERING_OMNI_OMNIVERSEMESH_HH

#include <ignition/rendering.hh>

namespace ignition::rendering::omni {
class OmniverseMesh : public Mesh {
 public:
  bool HasSkeleton() const override;
  std::map<std::string, math::Matrix4d> SkeletonLocalTransforms()
      const override;
  void SetSkeletonLocalTransforms(
      const std::map<std::string, math::Matrix4d> &_tfs) override;
  std::unordered_map<std::string, float> SkeletonWeights() const override;
  void SetSkeletonWeights(
      const std::unordered_map<std::string, float> &_weights) override;
  void SetSkeletonAnimationEnabled(const std::string &_name, bool _enabled,
                                   bool _loop = true,
                                   float _weight = 1.0) override;
  bool SkeletonAnimationEnabled(const std::string &_name) const override;
  void UpdateSkeletonAnimation(
      std::chrono::steady_clock::duration _time) override;
  unsigned int SubMeshCount() const override;
  bool HasSubMesh(ConstSubMeshPtr _subMesh) const override;
  bool HasSubMeshName(const std::string &_name) const override;
  SubMeshPtr SubMeshByName(const std::string &_name) const override;
  SubMeshPtr SubMeshByIndex(unsigned int _index) const override;
  void SetDescriptor(const MeshDescriptor &_desc) override;
  const MeshDescriptor &Descriptor() const override;
};

class OmniverseSubMesh : public SubMesh {
  MaterialPtr Material() const override;
  void SetMaterial(const std::string &_name, bool _unique = true) override;
  void SetMaterial(MaterialPtr _material, bool _unique = true) override;
};
}  // namespace ignition::rendering::omni

#endif
