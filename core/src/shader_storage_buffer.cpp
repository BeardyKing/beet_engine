#include <beet/log.h>
#include <beet/shader_storage_buffer.h>

namespace beet {

const void ShaderStorageBuffer::init(const vec2i size) {
    init_ssbo_oit(size);
    resize(size);
}

const void ShaderStorageBuffer::resize(const vec2i size) {
    glDeleteTextures(1, &m_fragmentStartIndices);
    glCreateTextures(GL_TEXTURE_2D, 1, &m_fragmentStartIndices);
    glTextureStorage2D(m_fragmentStartIndices, 1, GL_R32UI, size.x, size.y);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(1, m_fragmentStartIndices, 0, false, 0, GL_READ_WRITE, GL_R32UI);

    std::vector<uint> fragmentIndexInit(size.x * size.y, 0xFFFFFFFF);
    glNamedBufferData(m_fragmentStartIndicesInit, fragmentIndexInit.size() * sizeof(uint), fragmentIndexInit.data(),
                      GL_DYNAMIC_COPY);

    size_t ssboSize = size.x * size.y * MAX_FRAGMENTS * sizeof(Fragment);
    glNamedBufferData(m_fragmentBuffer, ssboSize, nullptr, GL_DYNAMIC_DRAW);

    log::info("SSBO INFO: {},{}, Max fragments: {}, size of Fragment: {}", size.x, size.y, MAX_FRAGMENTS,
              sizeof(Fragment));
    log::info("SSBO size: {} GiB", 1024.0f * 1024.0f * 1024.0f * ssboSize);
}

const void ShaderStorageBuffer::init_ssbo_oit(const vec2i size) {
    glCreateBuffers(1, &m_fragmentStartIndicesInit);
    glCreateBuffers(1, &m_fragmentBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_fragmentBuffer);

    glCreateBuffers(1, &m_atomicFragmentIndex);
    glNamedBufferData(m_atomicFragmentIndex, sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, m_atomicFragmentIndex);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_fragmentStartIndices);
    glTextureStorage2D(m_fragmentStartIndices, 1, GL_R32UI, size.x, size.y);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_fragmentStartIndices, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

const void ShaderStorageBuffer::bind_oit(const vec2i size) const {
    glBindImageTexture(6, m_fragmentStartIndices, 0, false, 0, GL_READ_WRITE, GL_R32UI);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_fragmentStartIndicesInit);
    glTextureSubImage2D(m_fragmentStartIndices, 0, 0, 0, size.x, size.y, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    uint atomicFragmentIndex = 0;
    glNamedBufferSubData(m_atomicFragmentIndex, 0, sizeof(uint), &atomicFragmentIndex);
}

const void ShaderStorageBuffer::unbind() const {
    glBindImageTexture(1, m_fragmentStartIndices, 0, false, 0, GL_READ_WRITE, GL_R32UI);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
}  // namespace beet
