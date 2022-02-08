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
#ifndef OMNIVERSE_FUSDNOTICELISTENER_HPP
#define OMNIVERSE_FUSDNOTICELISTENER_HPP

#include <pxr/usd/usd/notice.h>
#include <pxr/usd/sdf/path.h>

#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/sphere.h>

namespace ignition
{
namespace omniverse
{
class FUSDNoticeListener : public pxr::TfWeakBase
{
 public:
  FUSDNoticeListener(Scene::SharedPtr &_scene, const std::string &_worldName)
      : scene(_scene), worldName(_worldName)
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
    if (!_prim) return;
    auto children = _prim.GetChildren();
    for (const pxr::UsdPrim &childPrim : children)
    {
      if (!childPrim) continue;
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
    for (const pxr::SdfPath &Path : ObjectsChanged.GetResyncedPaths())
    {
      ignmsg << "Resynced Path: " << Path.GetText() << std::endl;
      auto modelUSD = this->scene->GetPrimAtPath(std::string(Path.GetText()));
      // if (modelUSD)
      // {
      // 	std::cerr << "Model is here" << '\n';
      //
      // 	std::string sdfString = std::string("<sdf version='1.7'>\n");
      //
      // 	sdfString += std::string("\t<model name='") +
      // 		modelUSD.GetPath().GetName() + std::string("'>\n");
      //
      // 	sdfString += "\t\t\t<pose>" +
      //     std::to_string(3) + " " +
      //     std::to_string(3) + " " +
      //     std::to_string(0.5) + " " +
      //     std::to_string(0) + " " +
      //     std::to_string(0) + " " +
      //     std::to_string(0) + "</pose>\n";
      //
      // 	sdfString += "\t\t<link name='" + modelUSD.GetPath().GetName()+
      // "_link'>\n";
      //
      // 	createSDF(sdfString, modelUSD);
      //
      // 	sdfString += "\t\t</link>\n";
      //
      // 	sdfString += std::string("\t</model>\n</sdf>\n");
      //
      // 	std::cerr << sdfString;
      //
      // 	// Prepare the input parameters.
      // 	ignition::msgs::EntityFactory req;
      // 	req.set_sdf(sdfString);
      // 	req.set_name(modelUSD.GetPath().GetName());
      // 	req.set_allow_renaming(false);
      //
      //   ignition::msgs::Boolean rep;
      //   bool result;
      //   unsigned int timeout = 5000;
      //   bool executed = node.Request(
      //     "/world/" + worldName + "/create", req, timeout, rep, result);
      // 	if (executed)
      //   {
      // 		if (rep.data())
      // 		{
      // 			std::cerr << "model was inserted" << '\n';
      // 		}
      // 		else
      // 		{
      // 			std::cerr << "Error model was not inserted" <<
      // '\n';
      // 		}
      // 	}
      // }
      // else
      // {
      // 	std::cerr << "Model is not yet here" << '\n';
      // }
    }
    for (const pxr::SdfPath &Path : ObjectsChanged.GetChangedInfoOnlyPaths())
    {
      ignmsg << "Changed Info Path: " << Path.GetText() << std::endl;
    }
  }
  Scene::SharedPtr scene;
  std::string worldName;
  ignition::transport::Node node;
};
}  // namespace omniverse
}  // namespace ignition

#endif
