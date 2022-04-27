#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <beet/window.h>

namespace beet {
class Engine {
   public:
    Engine();
    ~Engine();

    void update_modules();
    bool is_open();

   private:
    std::vector<std::shared_ptr<Subsystem>> m_engineModules;

    std::shared_ptr<Window> m_windowModule;
};

}  // namespace beet
