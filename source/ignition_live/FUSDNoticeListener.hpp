/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
#ifndef IGNITION_OMNIVERSE_FUSDNOTICELISTENER_HPP
#define IGNITION_OMNIVERSE_FUSDNOTICELISTENER_HPP

#include "GetOp.hpp"

#include <ignition/common/Console.hh>

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>

namespace ignition
{
namespace omniverse
{
class FUSDNoticeListener : public pxr::TfWeakBase
{
 public:
  FUSDNoticeListener(
    std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> _stage,
    const std::string &_worldName)
      : stage(_stage), worldName(_worldName)
  {
  }

  void ParseCube(const pxr::UsdPrim &_prim, std::string &_stringSDF)
  {
    double size;
    auto variant_cylinder = pxr::UsdGeomCube(_prim);
    variant_cylinder.GetSizeAttr().Get(&size);

    double x, y, z;

    // TODO(ahcorde): Fix scale
    x = size;  // * _scale.X(),
    y = size;  // * _scale.Y(),
    z = size;  // * _scale.Z()

    _stringSDF +=
        "\t\t\t<visual name='" + _prim.GetPath().GetName() + "_visual'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<box>\n";
    _stringSDF += "\t\t\t\t\t\t<size>" + std::to_string(x) + " " +
                  std::to_string(y) + " " + std::to_string(z) + "</size>\n";
    _stringSDF += "\t\t\t\t\t</box>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</visual>\n";

    _stringSDF += "\t\t\t<collision name='" + _prim.GetPath().GetName() +
                  "_collision'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<box>\n";
    _stringSDF += "\t\t\t\t\t\t<size>" + std::to_string(x) + " " +
                  std::to_string(y) + " " + std::to_string(z) + "</size>\n";
    _stringSDF += "\t\t\t\t\t</box>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</collision>\n";
  }

  void ParseCylinder(const pxr::UsdPrim &_prim, std::string &_stringSDF)
  {
    auto variant_cylinder = pxr::UsdGeomCylinder(_prim);
    double radius;
    double height;
    variant_cylinder.GetRadiusAttr().Get(&radius);
    variant_cylinder.GetHeightAttr().Get(&height);

    _stringSDF +=
        "\t\t\t<visual name='" + _prim.GetPath().GetName() + "_visual'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<cylinder>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<radius>" + std::to_string(radius) + "</radius>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<height>" + std::to_string(height) + "</height>\n";
    _stringSDF += "\t\t\t\t\t</cylinder>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</visual>\n";

    _stringSDF += "\t\t\t<collision name='" + _prim.GetPath().GetName() +
                  "_collision'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<cylinder>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<radius>" + std::to_string(radius) + "</radius>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<height>" + std::to_string(height) + "</height>\n";
    _stringSDF += "\t\t\t\t\t</cylinder>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</collision>\n";
  }

  void ParseSphere(const pxr::UsdPrim &_prim, std::string &_stringSDF)
  {
    double radius;
    auto variant_sphere = pxr::UsdGeomSphere(_prim);
    variant_sphere.GetRadiusAttr().Get(&radius);
    _stringSDF +=
        "\t\t\t<visual name='" + _prim.GetPath().GetName() + "_visual'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<sphere>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<radius>" + std::to_string(radius) + "</radius>\n";
    _stringSDF += "\t\t\t\t\t</sphere>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</visual>\n";

    _stringSDF += "\t\t\t<collision name='" + _prim.GetPath().GetName() +
                  "_collision'>\n";
    _stringSDF += "\t\t\t\t<geometry>\n";
    _stringSDF += "\t\t\t\t\t<sphere>\n";
    _stringSDF +=
        "\t\t\t\t\t\t<radius>" + std::to_string(radius) + "</radius>\n";
    _stringSDF += "\t\t\t\t\t</sphere>\n";
    _stringSDF += "\t\t\t\t</geometry>\n";
    _stringSDF += "\t\t\t</collision>\n";
  }

  void CreateSDF(std::string &_stringSDF, const pxr::UsdPrim &_prim)
  {
    if (!_prim)
      return;
    auto children = _prim.GetChildren();
    for (const pxr::UsdPrim &childPrim : children)
    {
      igndbg << childPrim.GetPath().GetText() << "\n";
      if (!childPrim)
        continue;
      if (childPrim.IsA<pxr::UsdGeomSphere>())
      {
        ParseSphere(childPrim, _stringSDF);
      }
      else if (childPrim.IsA<pxr::UsdGeomCylinder>())
      {
        ParseCylinder(childPrim, _stringSDF);
      }
      else if (childPrim.IsA<pxr::UsdGeomCube>())
      {
      }
      else
      {
        CreateSDF(_stringSDF, childPrim);
      }
    }
  }

  void Handle(const class pxr::UsdNotice::ObjectsChanged &ObjectsChanged)
  {
    for (const pxr::SdfPath &objectsChanged : ObjectsChanged.GetResyncedPaths())
    {
      ignmsg << "Resynced Path: " << objectsChanged.GetText() << std::endl;
      auto stage = this->stage->Lock();
      auto modelUSD = stage->GetPrimAtPath(objectsChanged);
      if (modelUSD)
      {
        std::string strPath = objectsChanged.GetText();
        if (strPath.find("_link") != std::string::npos
           || strPath.find("_visual") != std::string::npos
           || strPath.find("geometry") != std::string::npos) {
          return;
        }
      	std::string sdfString = std::string("<sdf version='1.7'>\n");

      	sdfString += std::string("\t<model name='") +
      		modelUSD.GetPath().GetName() + std::string("'>\n");

      	sdfString += "\t\t<pose>" +
          std::to_string(3) + " " +
          std::to_string(3) + " " +
          std::to_string(0.5) + " " +
          std::to_string(0) + " " +
          std::to_string(0) + " " +
          std::to_string(0) + "</pose>\n";

      	sdfString += "\t\t<link name='" + modelUSD.GetPath().GetName() + "_link'>\n";

      	CreateSDF(sdfString, modelUSD);

      	sdfString += "\t\t</link>\n";

      	sdfString += std::string("\t</model>\n</sdf>\n");

      	std::cerr << sdfString;

      	// Prepare the input parameters.
      	ignition::msgs::EntityFactory req;
      	req.set_sdf(sdfString);
      	req.set_name(modelUSD.GetPath().GetName());
      	req.set_allow_renaming(false);

        ignition::msgs::Boolean rep;
        bool result;
        unsigned int timeout = 5000;
        bool executed = node.Request(
          "/world/" + this->worldName + "/create",
          req, timeout, rep, result);
      	if (executed)
        {
      		if (rep.data())
      		{
      			std::cerr << "Model was inserted [" << modelUSD.GetPath().GetName()
                      << "]" << '\n';
      		}
      		else
      		{
      			std::cerr << "Error model was not inserted" << '\n';
      		}
      	}
      }
    }

    for (const pxr::SdfPath &objectsChanged :
        ObjectsChanged.GetChangedInfoOnlyPaths())
    {
      auto stage = this->stage->Lock();
      igndbg << "path " << objectsChanged.GetText() << std::endl;
      auto modelUSD = stage->GetPrimAtPath(objectsChanged.GetParentPath());
      auto property = modelUSD.GetPropertyAtPath(objectsChanged);
      std::string strProperty = property.GetBaseName().GetText();
      if (strProperty == "radius")
      {
        double radius;
        auto attribute = modelUSD.GetAttributeAtPath(objectsChanged);
        attribute.Get(&radius);
      }
      if (strProperty == "translate")
      {
        auto xform = pxr::UsdGeomXformable(modelUSD);

        auto transforms = GetOp(xform);

        // Prepare the input parameters.
        ignition::msgs::Pose req;
        ignition::msgs::Boolean rep;

        req.set_name(modelUSD.GetPath().GetName());

        req.mutable_position()->set_x(transforms.position[0]);
        req.mutable_position()->set_y(transforms.position[1]);
        req.mutable_position()->set_z(transforms.position[2]);

        ignition::math::Quaterniond q(
          transforms.rotXYZ[0],
          transforms.rotXYZ[1],
          transforms.rotXYZ[2]);

        req.mutable_orientation()->set_x(q.X());
        req.mutable_orientation()->set_y(q.Y());
        req.mutable_orientation()->set_z(q.Z());
        req.mutable_orientation()->set_w(q.W());

        bool result;
        unsigned int timeout = 500;
        bool executed = this->node.Request(
        	"/world/" + this->worldName + "/set_pose",
          req, timeout, rep, result);
        if (executed)
        {
          if (!result)
            ignerr << "Service call failed" << std::endl;
        }
        else
          ignerr << "Service call timed out" << std::endl;
      }
    }
  }

  std::shared_ptr<ThreadSafe<pxr::UsdStageRefPtr>> stage;
  std::string worldName;
  ignition::transport::Node node;
};
}  // namespace omniverse
}  // namespace ignition

#endif
