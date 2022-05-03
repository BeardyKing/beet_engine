#pragma once

#include <string>
#include "spdlog/spdlog.h"

namespace beet {

namespace log {

using namespace spdlog;

class Logger {
   public:
    static void setup();
};

}  // namespace log

}  // namespace beet
