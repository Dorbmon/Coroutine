#pragma once
#include <coroutine>
#include <type_traits>

namespace RCo {
template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};
class RWorker;
RWorker *__getCurrentWorker();
} // namespace RCo