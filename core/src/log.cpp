#include <beet/log.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/sinks/basic_file_sink.h"

namespace beet {

namespace log {

void Logger::setup() {
#ifdef BEET_DEBUG
    log::set_level(spdlog::level::debug);
#endif

    log::default_logger()->sinks().clear();
    log::default_logger()->sinks().push_back(std::make_shared<log::sinks::stdout_color_sink_st>());
    auto sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt");
    log::default_logger()->sinks().push_back(sharedFileSink);
    // %R: 24-hour time (HH:MM)/(Hour:Minutes)
    // %S: seconds
    // %e: milliseconds
    // %^: start color
    // %l: level
    // %$: end color
    // %v: message
    log::set_pattern("[%R:%S:%e] [%^%l%$]: %v");
}

}  // namespace log

}  // namespace beet
