#include <iostream>
#include <thread>
#include <cassert>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>

#include "ruin_mutex.h"

using namespace std;

const int a = 1000;
const int b = 10;

struct Data {
  int count{};
  std::string message;
};

int main() {
  cout << "Hello World!" << endl;

  ruin::Mutex<Data> data;
  const auto w = [&data](int n){
    for (int i = 0; i < a; ++i) {
      data.Write([n](auto& data){
        data.count++;
        data.message = std::to_string(n);
      });
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i<b; ++i) {
    threads.emplace_back([w, i]{ w(i); });
  }

  std::atomic_bool is_done(false);
  std::thread tr([&data, &is_done]{
    int i{};
    do {
      data.Read([&i](const auto& data) {
        cout << to_string(i++) << ": " << data.count << ", " << data.message << endl;
      });
      using namespace std::chrono_literals;
      this_thread::sleep_for(500ns);
    } while (!is_done);
  });

  for (auto& t : threads) {
    t.join();
  }
  is_done = true;
  tr.join();

  auto result = data.Clone();
  cout << "Done: " << result.count << ", " << result.message << endl;
  assert(result.count == a*b);

  return 0;
}
