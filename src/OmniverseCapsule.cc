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

#include "OmniverseCapsule.hh"

#include <exception>

namespace ignition::rendering::omni {
void OmniverseCapsule::SetRadius(double _radius) {
  throw std::runtime_error("not implemented");
}
void OmniverseCapsule::SetLength(double _length) {
  throw std::runtime_error("not implemented");
}
double OmniverseCapsule::Radius() const {
  throw std::runtime_error("not implemented");
}
double OmniverseCapsule::Length() const {
  throw std::runtime_error("not implemented");
}
}  // namespace ignition::rendering::omni
