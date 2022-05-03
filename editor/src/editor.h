#pragma once
#include <beet/engine.h>
#include <uuid.h>
#include <entt/entt.hpp>

namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::shared_ptr<Engine> m_engine;

    entt::registry m_registry;
    std::unique_ptr<uuids::uuid_random_generator> m_uuidGenerator;
};

}  // namespace beet
