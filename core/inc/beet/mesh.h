#pragma once

#include <beet/asset.h>
#include <beet/types.h>
#include <glad/glad.h>
#include <memory>
#include <string>
#include <vector>

namespace beet::components {
class IndexBuffer;
class VertexLayout;
}  // namespace beet::components

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

    void draw();

    GLuint get_vertex_buffer() { return m_vbo; }
    GLuint get_index_buffer() { return m_ibo; }

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

class VertexLayout {
   public:
    VertexLayout(vec3 position, vec3 normal, vec2 textureCoordinate, vec3 tangent)
        : m_position(position), m_normal(normal), m_uv(textureCoordinate), m_tangent(tangent) {}
    vec3 m_position;
    vec3 m_normal;
    vec2 m_uv;
    vec3 m_tangent;
};

}  // namespace components
}  // namespace beet
