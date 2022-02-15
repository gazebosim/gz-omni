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

#ifndef IGNITION_OMNIVERSE_THREADSAFE_HPP
#define IGNITION_OMNIVERSE_THREADSAFE_HPP

#include <mutex>

namespace ignition::omniverse
{

/// \brief Make an object threadsafe by locking it behind a mutex.
template <typename T, typename MutexT = std::recursive_mutex>
class ThreadSafe
{
 public:
  class Ref
  {
   public:
    Ref(T& _data, MutexT& _m);
    ~Ref();

    // don't allow copying and moving
    Ref(const Ref&) = delete;
    Ref(Ref&&) = delete;
    Ref& operator=(const Ref&) = delete;

    T& operator*();
    T& operator->();

   private:
    MutexT& m;
    T& data;
  };

  /// \brief Takes ownership of the data.
  explicit ThreadSafe(T&& _data);

  // don't allow copying
  ThreadSafe(const ThreadSafe&) = delete;
  ThreadSafe& operator=(const ThreadSafe&) = delete;

  // moving is ok
  ThreadSafe(ThreadSafe&&) = default;

  /// \brief Locks the mutex
  Ref Lock();

 private:
  T data;
  MutexT mutex;
};

template <typename T, typename MutexT>
ThreadSafe<T, MutexT>::Ref::Ref(T& _data, MutexT& _m) : data(_data), m(_m)
{
  this->m.lock();
}

template <typename T, typename MutexT>
ThreadSafe<T, MutexT>::Ref::~Ref()
{
  this->m.unlock();
}

template <typename T, typename MutexT>
T& ThreadSafe<T, MutexT>::Ref::operator*()
{
  return this->data;
}

template <typename T, typename MutexT>
T& ThreadSafe<T, MutexT>::Ref::operator->()
{
  return this->data;
}

template <typename T, typename MutexT>
ThreadSafe<T, MutexT>::ThreadSafe(T&& _data) : data(_data)
{
}

template <typename T, typename MutexT>
typename ThreadSafe<T, MutexT>::Ref ThreadSafe<T, MutexT>::Lock()
{
  return Ref(this->data, this->mutex);
}

}  // namespace ignition::omniverse

#endif
