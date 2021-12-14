#pragma once

#include <string>

#include "ruin_mutex.h"
using namespace ruin;
using namespace std;

namespace test {

#define TEST(name) bool test_##name()

TEST(constructor_default) {
  Mutex<string> m;
  return m.Clone().empty();
}

TEST(constructor_from_lvalue) {
  string s("Hello");
  Mutex<string> m(s);
  return m.Clone() == s;
}

TEST(constructor_from_rvalue) {
  auto c = "Hello";
  string s(c);
  Mutex<string> m(std::move(s));
  return m.Clone() == c && s.empty();
}

TEST(constructor_emplace) {
  Mutex<string> m(5, 'A');
  return m.Clone() == "AAAAA";
}

TEST(read_data) {
  Mutex<string> m("Hello");
  size_t s{};
  m.Read([&s](const string& data){
    s = data.size();
  });
  return s != 0 && s == m.Clone().size();
}

TEST(write_data) {
  Mutex<string> m("Hello");
  string w("world");
  m.Write([w](string& data) {
    data += string(" ") + w;
  });
  return m.Clone() == "Hello world";
}

#define RUN_TEST(name) { \
bool is_ok = test_##name(); \
cout << "Test \"" << #name << "\" ... " << (is_ok ? "OK" : "FAIL") << endl; \
}

void run_all() {
  RUN_TEST(constructor_default);
  RUN_TEST(constructor_from_lvalue);
  RUN_TEST(constructor_from_rvalue);
  RUN_TEST(constructor_emplace);
  RUN_TEST(read_data);
  RUN_TEST(write_data);
}

} // namespace test
