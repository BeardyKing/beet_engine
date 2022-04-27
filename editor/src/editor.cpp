#include "editor.h"
#include <beet/log.h>

namespace beet {

Editor::Editor() {
    log::Logger::setup();
    m_engine = std::make_unique<Engine>();
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
