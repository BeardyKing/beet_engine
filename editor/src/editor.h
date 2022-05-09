#pragma once
#include <beet/engine.h>
#include <beet/scene.h>

namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::unique_ptr<Engine> m_engine;
    std::unique_ptr<Scene> m_scene;
};

}  // namespace beet
