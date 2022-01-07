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

#include <pxr/usd/usdGeom/mesh.h>

#include <ignition/rendering/base/BaseStorage.hh>

#include "OmniverseVisual.hh"

namespace ignition::rendering::omni {

OmniverseMesh::SharedPtr OmniverseMesh::Make(unsigned int _id,
                                             const std::string& _name,
                                             OmniverseScene::SharedPtr _scene,
                                             const MeshDescriptor& _desc) {
  auto sp = std::shared_ptr<OmniverseMesh>(new OmniverseMesh());
  sp->InitObject(_id, _name, _scene);
  sp->subMeshes = std::make_shared<BaseSubMeshStore<OmniverseSubMesh>>();
  for (size_t i = 0; i < _desc.mesh->SubMeshCount(); i++) {
    auto ovSubMesh = OmniverseSubMesh::Make(_id, _name, _scene);
    sp->subMeshes->Add(ovSubMesh);
  }

  // based on SDF -> USD converter PR by adlarkin.
  auto ignMesh = _desc.mesh;
  for (unsigned int i = 0; i < ignMesh->SubMeshCount(); ++i) {
    auto subMesh = ignMesh->SubMeshByIndex(i).lock();
    if (!subMesh) {
      ignerr << "Unable to get a shared pointer to submesh at index [" << i
             << "] of parent mesh [" << ignMesh->Name() << "]\n";
      return nullptr;
    }

    // copy the submesh's vertices to the usd mesh's "points" array
    for (unsigned int v = 0; v < subMesh->VertexCount(); ++v) {
      const auto& vertex = subMesh->Vertex(v);
      sp->meshPoints.push_back(
          pxr::GfVec3f(vertex.X(), vertex.Y(), vertex.Z()));
    }

    // copy the submesh's indices to the usd mesh's "this->faceVertexIndices"
    // array
    for (unsigned int j = 0; j < subMesh->IndexCount(); ++j)
      sp->faceVertexIndices.push_back(subMesh->Index(j));

    // set the usd mesh's "this->faceVertexCounts" array according to
    // the submesh primitive type
    // TODO(adlarkin) support all primitive types. The computations are more
    // involved for LINESTRIPS, TRIFANS, and TRISTRIPS. I will need to spend
    // some time deriving what the number of faces for these primitive types
    // are, given the number of indices. The "this->faceVertexCounts" array will
    // also not have the same value for every element in the array for these
    // more complex primitive types (see the TODO note in the for loop below)
    unsigned int verticesPerFace = 0;
    unsigned int numFaces = 0;
    switch (subMesh->SubMeshPrimitiveType()) {
      case ignition::common::SubMesh::PrimitiveType::POINTS:
        verticesPerFace = 1;
        numFaces = subMesh->IndexCount();
        break;
      case ignition::common::SubMesh::PrimitiveType::LINES:
        verticesPerFace = 2;
        numFaces = subMesh->IndexCount() / 2;
        break;
      case ignition::common::SubMesh::PrimitiveType::TRIANGLES:
        verticesPerFace = 3;
        numFaces = subMesh->IndexCount() / 3;
        break;
      case ignition::common::SubMesh::PrimitiveType::LINESTRIPS:
      case ignition::common::SubMesh::PrimitiveType::TRIFANS:
      case ignition::common::SubMesh::PrimitiveType::TRISTRIPS:
      default:
        std::cerr << "Submesh " << subMesh->Name()
                  << " has a primitive type that is not supported.\n";
        return nullptr;
    }
    // TODO(adlarkin) update this loop to allow for varying element
    // values in the array (see TODO note above). Right now, the
    // array only allows for all elements to have one value, which in
    // this case is "verticesPerFace"
    for (unsigned int n = 0; n < numFaces; ++n)
      sp->faceVertexCounts.push_back(verticesPerFace);
  }

  return sp;
}

bool OmniverseMesh::AttachToVisual(VisualPtr _visual) {
  auto ovVisual = std::dynamic_pointer_cast<OmniverseVisual>(_visual);
  if (!ovVisual) {
    ignerr << "Failed to attach to visual '" << _visual->Id()
           << "' (not an omniverse visual)" << std::endl;
    return false;
  }

  auto path = ovVisual->Gprim().GetPath().AppendPath(
      pxr::SdfPath(NameToSdfPath(this->Name())));
  this->gprim = pxr::UsdGeomMesh::Define(this->Stage(), path);
  auto& usdMesh = static_cast<pxr::UsdGeomMesh&>(this->gprim);
  usdMesh.CreatePointsAttr().Set(this->meshPoints);
  usdMesh.CreateFaceVertexIndicesAttr().Set(this->faceVertexIndices);
  usdMesh.CreateFaceVertexCountsAttr().Set(this->faceVertexCounts);
  return true;
}

OmniverseSubMesh::SharedPtr OmniverseSubMesh::Make(
    unsigned int _id, const std::string& _name,
    OmniverseScene::SharedPtr _scene) {
  auto sp = std::shared_ptr<OmniverseSubMesh>(new OmniverseSubMesh());
  sp->InitObject(_id, _name, _scene);
  return sp;
}

void OmniverseSubMesh::SetMaterialImpl(MaterialPtr _material) {
  // TODO: implement
}

}  // namespace ignition::rendering::omni
