#pragma once

#include <functional>

namespace RCo {

class RWorker;
RWorker *__getCurrentWorker();

template<class T, typename RET_V>
concept FunctionWithCallback = requires (std::function<RET_V()> callback) {
    T(callback);
};
} // namespace RCo