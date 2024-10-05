#pragma once
#include <coroutine>
#include <type_traits>
#include <functional>

namespace RCo {
template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};


class RWorker;
RWorker *__getCurrentWorker();


#define CALLBACK_TO_AWAITABLE(f)
template<class T, typename RET_V>
concept FunctionWithCallback = requires (std::function<RET_V()> callback) {
    T(callback);
};
} // namespace RCo