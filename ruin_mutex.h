#pragma once

#include <mutex>
#include <type_traits>

namespace ruin {

template<typename T>
class Mutex {
public:
  Mutex() = default;
  Mutex(T&& data);

  T Clone();

  template<typename F>
  void Read(F);
  template<typename F>
  void Write(F);

private:
  std::mutex _mutex;
  T _data;
};

//--- Implementations ---

template<typename T>
Mutex<T>::Mutex(T&& data)
    : _data(std::forward<T>(data)) {
}

template<typename T>
T Mutex<T>::Clone() {
  _mutex.lock();
  T clone = _data;
  _mutex.unlock();
  return clone;
}

template<typename T>
template<typename F>
void Mutex<T>::Read(F f) {
  _mutex.lock();
  f(std::add_const_t<std::add_lvalue_reference_t<T>>(_data));
  _mutex.unlock();
}

template<typename T>
template<typename F>
void Mutex<T>::Write(F f) {
  _mutex.lock();
  f(std::add_lvalue_reference_t<T>(_data));
  _mutex.unlock();
}

} // namespace ruin
