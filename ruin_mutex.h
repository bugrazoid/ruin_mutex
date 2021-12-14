#pragma once

#include <mutex>
#include <type_traits>

namespace ruin {

template<typename T>
class Mutex {
public:
  Mutex() = default;
  Mutex(T data);
  template<typename ...Args>
  Mutex(Args...args);

  T Clone();

  template<typename F>
  void Read(F);
  template<typename F>
  void Write(F);

private:
  std::mutex _mutex;
  using ConstrainedData = std::enable_if_t<
                            !std::is_reference_v<T> &&
                            !std::is_pointer_v<T>,
                          T>;
  ConstrainedData _data;
};

//--- Implementations ---

template<typename T>
Mutex<T>::Mutex(T data)
    : _data(data) {
}

template<typename T>
template<typename ...Args>
Mutex<T>::Mutex(Args ...args)
    : _data(args...) {
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
