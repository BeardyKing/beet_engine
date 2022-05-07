#pragma once
#include "beet/asset.h"
#include "beet/asset_manager.h"
#include "beet/component.h"
#include "beet/mesh.h"
#include "beet/types.h"

#include <string>
#include <vector>

namespace beet {
namespace components {

class InstanceMesh : public Component<InstanceMesh> {
   public:
    InstanceMesh();
    InstanceMesh(const std::string& path);

    void on_awake() override;
    void draw();

   private:
    std::shared_ptr<components::Mesh> m_mesh;
    std::string m_path;
};
}  // namespace components

}  // namespace beet
