#include <beet/instance_mesh.h>

namespace beet {
namespace components {
InstanceMesh::InstanceMesh() {}
InstanceMesh::InstanceMesh(const std::string& path) : m_path(path) {}

void InstanceMesh::on_awake() {
    m_mesh = AssetManager::load_asset<components::Mesh>(m_path).lock();
}
void InstanceMesh::draw() {
    m_mesh->draw();
}
}  // namespace components
}  // namespace beet
