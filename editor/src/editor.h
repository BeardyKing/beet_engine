#pragma once
#include <beet/engine.h>
#include <beet/scene.h>

namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::shared_ptr<Engine> m_engine;
    std::shared_ptr<Scene> m_scene;
};

}  // namespace beet
