#pragma once

#include <mutex>
#include <type_traits>

/// ruin is rust inspired
namespace ruin {

/** Mutex is inspired by rust mutex.
 *
 * Main features of this implementation:
 * 1. Mutex owns data that it protects
 * 2. Implementing access to data through functors
 * This allows the programmer to clearly understand what data are protected and does not make it
 * difficult to change them without blocking.
 */
template<typename T>
class Mutex {
public:
  /// Default constructor
  Mutex() = default;

  /// Construct from existed data
  Mutex(T data);

  /// Construct data emplace
  template<typename ...Args>
  Mutex(Args...args);

  /// Clone data
  T Clone();

  /** Read data with functor
   *
   * @param f - Functor used to read data with the signature f(const T&)
   *
   * To read the data convenient to use the lambda with the closure. The closure you need to
   * transfer a reference to the destination variable.
   *
   * @warning
   * Do not use pointers to obtain addresses for secure data, it may result in data racing.
   *
   * ### Example
   * ~~~
   *  struct Data {
   *    int count{};
   *    std::string message;
   *  };
   *
   *  ruin::Mutex<Data> m;
   *
   *  int main() {
   *    std::string dst;
   *    m.Read([&dst](const Data& data) {
   *      dst = data.message;
   *    });
   *  };
   * ~~~
   */
  template<typename F>
  void Read(F f);

  /** Write data with functor
   *
   * @param f - Functor used to write data with the signature f(T&)
   *
   * To write the data convenient to use the lambda with the closure.
   *
   * @warning
   * Do not use pointers to obtain addresses for secure data, it may result in data racing.
   *
   * ### Example
   * ~~~
   *  struct Data {
   *    int count{};
   *    std::string message;
   *  };
   *
   *  ruin::Mutex<Data> m;
   *
   *  int main() {
   *    std::string src;
   *    m.Write([&src](Data& data) {
   *      data.message = src;
   *    });
   *  };
   * ~~~
   */
  template<typename F>
  void Write(F f);

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
    : _data(std::move(data)) {
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
