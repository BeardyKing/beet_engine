#pragma once

#include <beet/asset.h>
#include <map>
#include <string>
#include "glad/glad.h"

namespace beet {
namespace components {

class ShaderProgram : public Asset {
   public:
    ShaderProgram();
    ShaderProgram(const std::string& path);

    ~ShaderProgram();

    //=For ECS========
    void on_awake() override;
    void on_destroy() override;
    //=For Asset=======
    void generate_default_asset() override;
    //=================

    bool load_shader(const std::string& folderName,
                     const std::string& vertexShaderPath,
                     const std::string& fragmentShaderPath);

    void set_asset_name(const std::string& assetName) { m_assetName = assetName; }

   private:
    void create_program(std::string& vertexShaderPath, std::string& fragmentShaderPath);
    GLuint create_shader(const std::string& path, uint16_t glShaderType);
    void check_link_errors();
    void check_compile_errors(GLuint shader, uint16_t glShaderType);

    bool does_file_path_exist(const std::string& path);
    std::string get_cross_platform_path(const std::string& folderName, const std::string& fileName);
    std::string load_shader_text(const std::string& fileName);

    GLuint m_id = 0;
    std::map<std::string, GLint> m_UniformLocations;
};
}  // namespace components
}  // namespace beet