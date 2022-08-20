#pragma once

namespace beet {
class Subsystem {
   public:
    Subsystem() {}
    ~Subsystem() {}

    virtual void on_awake() {}

    virtual void on_update(double deltaTime) {}
    virtual void on_late_update() {}

    virtual void on_destroy() {}
};
}  // namespace beet
