// clang-format off
// only x64 using MSVC x86/x64 supported on other compilers
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
// clang-format on

#include <beet/log.h>
#include <beet/mesh.h>
#include <filesystem>

namespace beet {
namespace components {

Mesh::Mesh() : Asset{AssetType::Mesh, ""} {}
Mesh::Mesh(const std::string& path) : Asset{AssetType::Mesh, path} {}

Mesh::~Mesh() {}

void Mesh::on_awake() {
    if (m_assetState != AssetState::Finished) {
        m_assetState = AssetState::Loading;
        internal_load_obj(m_fullPath);
    }
}

void Mesh::on_destroy() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    if (m_ibo) {
        glDeleteBuffers(1, &m_ibo);
    }
    log::debug("removed mesh : {}", m_fullPath);
}

void Mesh::generate_default_asset() {
    m_assetState = AssetState::Loading;
    m_fullPath = "fallbackMesh";

    generate_cube();

    if (!m_vbo) {
        log::error("VBO : fallbackMesh failed to be created");
        m_assetState = AssetState::Failed;
        return;
    }

    if (!m_ibo) {
        log::error("IBO : fallbackMesh failed to be created");
        m_assetState = AssetState::Failed;
        return;
    }

    m_assetState = AssetState::Finished;
    log::info("Fallback created");
}

void Mesh::generate_plane() {
    // clang-format off
    m_vertexLayout.emplace_back(
        VertexLayout{
            vec3(-1.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(0.0f, 1.0f),
            vec3(0)});
    m_vertexLayout.emplace_back(
    VertexLayout{
            vec3(-1.0f, -1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(0.0f, 0.0f),
            vec3(0)});
    m_vertexLayout.emplace_back(
    VertexLayout{
            vec3(1.0f, -1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(1.0f, 0.0f),
            vec3(0)});
    m_vertexLayout.emplace_back(
        VertexLayout{
            vec3(-1.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(0.0f, 1.0f),
            vec3(0)});
    m_vertexLayout.emplace_back(
        VertexLayout{
            vec3(1.0f, -1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(1.0f, 0.0f),
            vec3(0)});
    m_vertexLayout.emplace_back(
        VertexLayout{
            vec3(1.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec2(1.0f, 1.0f),
            vec3(0)});
    // clang-format on

    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);
    m_indices.push_back(0);
    m_indices.push_back(2);
    m_indices.push_back(5);

    log::debug("Fallback mesh loaded");

    init_buffers();
}

bool Mesh::internal_load_obj(const std::string& path) {
    std::string fileName = PATH_MODELS + path;
    std::filesystem::path fs_path = fileName;

    if (!exists(fs_path)) {
        log::error(fileName + " - does not Exist");
        m_assetState = AssetState::Failed;
        return false;
    }

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                                   aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if (!scene) {
        log::error("Failed to load mesh : {}", fileName);
        log::debug("Generating default asset");
        generate_default_asset();
        return false;
    }

    const aiMesh* model = scene->mMeshes[0];
    log::warn(m_assetName);

    for (size_t i = 0; i < model->mNumVertices; i++) {
        aiVector3D position;
        aiVector3D normal;
        aiVector3D textureCoordinates{0, 0, 0};  // note assimp mTextureCoords are a vec3
        aiVector3D tangent{0, 0, 0};

        normal = model->mNormals[i];
        position = model->mVertices[i];

        if (model->HasTextureCoords(0)) {
            textureCoordinates = model->mTextureCoords[0][i];
        }

        if (model->HasTangentsAndBitangents()) {
            tangent = model->mTangents[i];
        }

        // clang-format off
        m_vertexLayout.emplace_back(
            VertexLayout{
                vec3(position.x, position.y, position.z),
                vec3(normal.x, normal.y, normal.z),
                vec2(textureCoordinates.x, textureCoordinates.y),
                vec3(tangent.x, tangent.y, tangent.z)
            }
        );
    }

    for (size_t i = 0; i < model->mNumFaces; i++) {
        const aiFace& face = model->mFaces[i];
        assert(face.mNumIndices == 3);
        m_indices.push_back(face.mIndices[0]);
        m_indices.push_back(face.mIndices[1]);
        m_indices.push_back(face.mIndices[2]);
    }

    log::info("Model loaded at : {}", fileName);

    init_buffers();

    return true;
}
void Mesh::init_buffers() {
    // clang-format off

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexLayout.size() * sizeof(VertexLayout), m_vertexLayout.data(),GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (GLvoid*)(sizeof(VertexLayout::m_position)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (GLvoid*)(sizeof(VertexLayout::m_position) + sizeof(VertexLayout::m_normal)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (GLvoid*)(sizeof(VertexLayout::m_position) + sizeof(VertexLayout::m_normal) + sizeof(VertexLayout::m_uv)));

    // clang-format on

    if (!m_indices.empty()) {
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_assetState = AssetState::Finished;
    log::debug("Buffers generated at : {}", m_fullPath);
}

void Mesh::draw() {
    if (m_assetState != AssetState::Finished) {
        return;
    }

    glBindVertexArray(m_vao);
    if (!m_indices.empty()) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexLayout.size());
    }
    glBindVertexArray(0);
}

void Mesh::generate_cube() {
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, 1.000000), vec3(-1.000000, 0.000000, 0.000000),
                                             vec2(0.000000, 3.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, 1.000000), vec3(-1.000000, 0.000000, 0.000000),
                                             vec2(1.000000, 3.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, -1.000000), vec3(-1.000000, 0.000000, 0.000000),
                                             vec2(1.000000, 2.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, -1.000000), vec3(-1.000000, 0.000000, 0.000000),
                                             vec2(0.000000, 2.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000),
                                             vec2(0.000000, 2.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000),
                                             vec2(1.000000, 2.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000),
                                             vec2(1.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000),
                                             vec2(0.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, -1.000000), vec3(1.000000, 0.000000, 0.000000),
                                             vec2(0.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, -1.000000), vec3(1.000000, 0.000000, 0.000000),
                                             vec2(1.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, 1.000000), vec3(1.000000, 0.000000, 0.000000),
                                             vec2(1.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, 1.000000), vec3(1.000000, 0.000000, 0.000000),
                                             vec2(0.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000),
                                             vec2(0.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000),
                                             vec2(1.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000),
                                             vec2(1.000000, -1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000),
                                             vec2(0.000000, -1.000000), vec3(0.000000, 1.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, -1.000000), vec3(0.000000, -1.000000, 0.000000),
                                             vec2(-1.000000, 1.000000), vec3(1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, -1.000000), vec3(0.000000, -1.000000, 0.000000),
                                             vec2(0.000000, 1.000000), vec3(1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, -1.000000, 1.000000), vec3(0.000000, -1.000000, 0.000000),
                                             vec2(0.000000, 0.000000), vec3(1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, -1.000000, 1.000000), vec3(0.000000, -1.000000, 0.000000),
                                             vec2(-1.000000, 0.000000), vec3(1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, -1.000000), vec3(0.000000, 1.000000, 0.000000),
                                             vec2(1.000000, 1.000000), vec3(-1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, -1.000000), vec3(0.000000, 1.000000, 0.000000),
                                             vec2(2.000000, 1.000000), vec3(-1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(-1.000000, 1.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000),
                                             vec2(2.000000, 0.000000), vec3(-1.000000, 0.000000, 0.000000)});
    m_vertexLayout.emplace_back(VertexLayout{vec3(1.000000, 1.000000, 1.000000), vec3(0.000000, 1.000000, 0.000000),
                                             vec2(1.000000, 0.000000), vec3(-1.000000, 0.000000, 0.000000)});

    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);

    m_indices.push_back(0);
    m_indices.push_back(2);
    m_indices.push_back(3);

    m_indices.push_back(4);
    m_indices.push_back(5);
    m_indices.push_back(6);

    m_indices.push_back(4);
    m_indices.push_back(6);
    m_indices.push_back(7);

    m_indices.push_back(8);
    m_indices.push_back(9);
    m_indices.push_back(10);

    m_indices.push_back(8);
    m_indices.push_back(10);
    m_indices.push_back(11);

    m_indices.push_back(12);
    m_indices.push_back(13);
    m_indices.push_back(14);

    m_indices.push_back(12);
    m_indices.push_back(14);
    m_indices.push_back(15);

    m_indices.push_back(16);
    m_indices.push_back(17);
    m_indices.push_back(18);

    m_indices.push_back(16);
    m_indices.push_back(18);
    m_indices.push_back(19);

    m_indices.push_back(20);
    m_indices.push_back(21);
    m_indices.push_back(22);

    m_indices.push_back(20);
    m_indices.push_back(22);
    m_indices.push_back(23);

    log::debug("Generated cube");

    init_buffers();
}

}  // namespace components
}  // namespace beet
