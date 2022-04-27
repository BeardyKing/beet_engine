#pragma once

#include <beet/log.h>

#if defined(BEET_DEBUG)

#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#define BEET_DEBUG_BREAK() raise(SIGTRAP)
#elif defined(_WIN32)
#define BEET_DEBUG_BREAK() __debugbreak()
#endif  // OS

#if __has_include("spdlog/spdlog.h")
#define BEET_LOG_ERROR(...) spdlog::error(__VA_ARGS__);
#define BEET_LOG_CURRENT_POSITION() \
    BEET_LOG_ERROR("Assertion triggered at:\n\t{}:{}:{}", __FILE__, __FUNCTION__, __LINE__);
#else
#include <iostream>
inline void BEET_LOG_ERROR() {}
template <typename First, typename... Rest>
inline void BEET_LOG_ERROR(First&& first, Rest&&... rest) {
    std::cerr << std::forward<First>(first);
    BEET_LOG_ERROR(std::forward<Rest>(rest)...);
}

#define BEET_LOG_CURRENT_POSITION() \
    BEET_LOG_ERROR("Assertion triggered at:\n\t", __FILE__, ":", __FUNCTION__, ":", __LINE__, "\n");
#endif  // __has_include("spdlog/spdlog.h")

#define BEET_ASSERT_MESSAGE(condition, ...) \
    {                                       \
        if (!(condition)) {                 \
            BEET_LOG_CURRENT_POSITION();    \
            BEET_LOG_ERROR(__VA_ARGS__);    \
            BEET_DEBUG_BREAK();             \
        }                                   \
    }

#define BEET_ASSERT(condition)           \
    {                                    \
        if (!(condition)) {              \
            BEET_LOG_CURRENT_POSITION(); \
            BEET_DEBUG_BREAK();          \
        }                                \
    }

#else

#define BEET_ASSERT_MESSAGE(contition, ...)
#define BEET_ASSERT(condition)

#endif  // BEET_DEBUG
