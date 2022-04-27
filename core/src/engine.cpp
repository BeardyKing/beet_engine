#include <beet/engine.h>
#include <iostream>

namespace beet {

Engine::Engine() {}

Engine::~Engine() {}

void Engine::update_modules() {
    std::cout << "updated modules\n";
}

bool Engine::is_open() {
    return m_isOpen;
}
void Engine::close() {
    m_isOpen = false;
}

}  // namespace beet