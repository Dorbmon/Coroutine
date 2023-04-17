#pragma once
#include <coroutine>
namespace RCo {
template <typename Result>
class Task {
 public:
  Task(Result&& t);
};
}  // namespace RCo