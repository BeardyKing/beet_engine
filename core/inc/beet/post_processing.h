#pragma once
#include <beet/component.h>
#include <beet/shader_program.h>
#include <beet/types.h>
#include <glad/glad.h>

namespace beet {
namespace components {

class PostProcessing : public Component<PostProcessing> {
   public:
    //=For ECS========
    void on_awake() override;
    void on_destroy() override;
    //================
    //TODO when frame buffer manager impl get target texture from there and have it set in editor
    void set_target_texture(GLuint texture) { m_targetTexture = texture; };
    void apply_post_processing();

   private:
    GLuint m_targetTexture;
    ShaderProgram m_shader;
};
}  // namespace components
}  // namespace beet
