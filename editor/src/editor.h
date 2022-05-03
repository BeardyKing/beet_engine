#pragma once
#include <beet/engine.h>
#include <entt/entt.hpp>


namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::shared_ptr<Engine> m_engine;

    entt::registry m_registry;
};

}  // namespace beet
