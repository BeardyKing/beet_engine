#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace beet {
class Engine {
   public:
    Engine();
    ~Engine();

    void update_modules();
    bool is_open();
    void close();

   private:
    bool m_isOpen;
};

}  // namespace beet
