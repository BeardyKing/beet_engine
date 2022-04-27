#pragma once

#include <beet/engine.h>

namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::shared_ptr<Engine> m_engine;
};

}  // namespace beet
