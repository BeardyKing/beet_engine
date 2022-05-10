#include <beet/engine.h>
#include "../../editor/src/editor_widgets.h"

namespace beet {

Engine::Engine() {
    m_windowModule = std::make_shared<Window>(1920, 1080, "beetroot", *this);
    m_widgetModule = std::make_shared<EditorWidgets>(*this);
    m_assetManager = std::make_shared<AssetManager>();
    m_editorCameraModule = std::make_shared<EditorCameraController>(*this);
    m_rendererModule = std::make_shared<Renderer>(*this);

    // order dependent
    m_engineModules.emplace_back(m_windowModule);
    m_engineModules.emplace_back(m_assetManager);
    m_engineModules.emplace_back(m_editorCameraModule);
    m_engineModules.emplace_back(m_rendererModule);
    m_engineModules.emplace_back(m_widgetModule);

    for (auto& module : m_engineModules) {
        module->on_awake();
    }
}

void Engine::update_modules() {
    swap_frame();

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

void Engine::swap_frame() {
    m_windowModule->swap_frame();
}

}  // namespace beet