#include "editor.h"

namespace beet {

Editor::Editor() {
    m_engine = std::make_unique<Engine>();
    std::cout << "hello from editor constructor\n";
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
        m_engine->close();
    }
}

}  // namespace beet
