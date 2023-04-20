#pragma once
#include <type_traits>
namespace RCo {
template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};
#define __GetWorker co_await __getWorkerHelperFunction();

}  // namespace RCo