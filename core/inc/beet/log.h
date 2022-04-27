#pragma once

#include "spdlog/spdlog.h"
#include <string>

namespace beet {

namespace log {

using namespace spdlog;

class Logger {
   public:
    static void setup();
};

}  // namespace log

}  // namespace beet
