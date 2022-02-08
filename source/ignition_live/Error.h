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

#include <string>
#include <variant>

namespace ignition::omniverse
{
class Error
{
 public:
  const std::string message;

  explicit Error(const std::string& _message) : message(_message) {}

  friend std::ostream& operator<<(std::ostream& _output, const Error& _error)
  {
    _output << _error.message;
    return _output;
  }
};

template <typename T>
class MaybeError
{
 public:
  MaybeError(const T& _val) : data(_val) {}
  MaybeError(const ignition::omniverse::Error& _error) : data(_error) {}

  operator bool() const
  {
    return !std::holds_alternative<ignition::omniverse::Error>(this->data);
  }

  const T& Value() const { return std::get<T>(this->data); }

  const ignition::omniverse::Error& Error() const
  {
    return std::get<ignition::omniverse::Error>(this->data);
  }

 private:
  const std::variant<T, ignition::omniverse::Error> data;
};

}  // namespace ignition::omniverse
