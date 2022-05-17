#include <beet/post_processing.h>

namespace beet {
namespace components {

void PostProcessing::on_awake() {
    m_shader.load_shader("post_process", "post_process.vert", "post_process.frag");
}
void PostProcessing::on_destroy() {}

void PostProcessing::apply_post_processing() {
    glUseProgram(m_shader.get_program());

    // TODO impl reorderable effects list that
    // applies various post process effects to a set of temp frame buffers
    // then to the back buffer

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_targetTexture);
}

}  // namespace components
}  // namespace beet
