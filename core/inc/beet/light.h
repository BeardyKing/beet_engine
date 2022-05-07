#pragma once
#include <beet/component.h>
#include <beet/types.h>

namespace beet {
namespace components {

class PointLight : public Component<PointLight> {
   public:
    PointLight() = default;
    ~PointLight(){};

    //=For ECS========
    void on_awake(){};
    void on_destroy(){};
    //================

    float get_range() { return m_lightRange; };
    float get_intensity() { return m_lightIntensity; };
    vec3 get_color() { return m_color; };

    void set_range(float range) { m_lightRange = range; };
    void set_intensity(float intensity) { m_lightIntensity = intensity; };
    void set_color(vec3 color) { m_color = color; };

   private:
    float m_lightRange = 2.0f;
    float m_lightIntensity = 3;
    vec3 m_color = vec3(1.0f, 0.95f, 0.84f);
};

}  // namespace components
}  // namespace beet
