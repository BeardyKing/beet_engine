#pragma once

#include <beet/asset.h>
#include <beet/types.h>
#include <glad/glad.h>
#include <memory>
#include <string>
#include <vector>

namespace beet {
namespace components {

class Mesh : public Asset {
   public:
    Mesh();
    Mesh(const std::string& path);
    ~Mesh();

    //=For ECS========
    void on_awake() override;
    void on_destroy() override;
    //=For Asset=======
    void generate_default_asset() override;
    //=================
    void generate_cube();
    void generate_plane();

    void draw();

    GLuint get_vertex_buffer() { return m_vbo; }
    GLuint get_index_buffer() { return m_ibo; }

    const std::vector<VertexLayout>& get_vertex_layout() { return m_vertexLayout; };
    const std::vector<unsigned int>& get_indices() { return m_indices; };

   private:
    bool internal_load_obj(const std::string& path);
    void init_buffers();

   private:
    std::vector<VertexLayout> m_vertexLayout;
    std::vector<unsigned int> m_indices;
    size_t m_size;

    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ibo;
};

}  // namespace components
}  // namespace beet
