#include <beet/engine.h>

namespace beet {

Engine::Engine() {
    m_windowModule = std::make_shared<beet::Window>(1024, 768, "beetroot", *this);

    // order dependent
    m_engineModules.emplace_back(m_windowModule);

    for (auto& module : m_engineModules) {
        module->on_awake();
    }
}

void Engine::update_modules() {
    for (auto& module : m_engineModules) {
        module->on_update(m_windowModule->get_delta_time());
    }

    for (auto& module : m_engineModules) {
        module->on_late_update();
    }
}

Engine::~Engine() {
    for (auto& module : m_engineModules) {
        module->on_destroy();
    }
}

bool Engine::is_open() {
    return m_windowModule.get()->is_open();
}

}  // namespace beet