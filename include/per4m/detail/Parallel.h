//
// Created by daniele on 10/03/26.
//

#pragma once

#ifdef PER4M_THREAD_SAFE
#include <mutex>
#endif

namespace per4m::detail {
#ifdef PER4M_THREAD_SAFE
    using Mutex = std::mutex;
    using LockGuard = std::lock_guard<std::mutex>;
#else
    struct Mutex {};
    struct LockGuard {
        explicit LockGuard(Mutex &) {}
    };
#endif
} // namespace per4m::detail