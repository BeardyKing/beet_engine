#include <beet/shader_program.h>

#include <beet/log.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace beet {
namespace components {

ShaderProgram::ShaderProgram() : Asset{AssetType::Shader, ""} {}
ShaderProgram::ShaderProgram(const std::string& path) : Asset{AssetType::Shader, path} {}

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::on_destroy() {}

void ShaderProgram::on_awake() {}

bool ShaderProgram::load_shader(const std::string& folderName,
                                const std::string& vertexShaderPath,
                                const std::string& fragmentShaderPath) {
    std::string fullVertexPath = get_cross_platform_path(folderName, vertexShaderPath);
    std::string fullFragmentPath = get_cross_platform_path(folderName, fragmentShaderPath);

    bool has_valid_paths;
    has_valid_paths = does_file_path_exist(fullVertexPath);
    has_valid_paths = does_file_path_exist(fullFragmentPath);

    create_program(fullVertexPath, fullFragmentPath);

    return has_valid_paths;
}

void ShaderProgram::create_program(std::string& vertexShaderPath, std::string& fragmentShaderPath) {
    //=VS==============

    std::string vsString = load_shader_text(vertexShaderPath);
    const GLchar* vsSourcePtr = vsString.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSourcePtr, NULL);
    glCompileShader(vs);
    check_compile_errors(vs, GL_VERTEX_SHADER);

    //=FS==============

    std::string fsString = load_shader_text(fragmentShaderPath);
    const GLchar* fsSourcePtr = fsString.c_str();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSourcePtr, NULL);
    glCompileShader(fs);
    check_compile_errors(fs, GL_FRAGMENT_SHADER);

    //=PROGRAM=========
    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);

    check_link_errors();

    glDeleteShader(vs);
    glDeleteShader(fs);

    m_UniformLocations.clear();
}

std::string ShaderProgram::load_shader_text(const std::string& fileName) {
    std::stringstream ss;
    std::ifstream file;

    file.open(fileName, std::ios::in);

    if (!file.fail()) {
        ss << file.rdbuf();
    }

    file.close();

    return ss.str();
}

bool ShaderProgram::does_file_path_exist(const std::string& path) {
    std::filesystem::path vs_filesystem_path = std::filesystem::path(path);

    if (exists(vs_filesystem_path)) {
        log::debug("shader file loaded at : {}", path);
    } else {
        log::error("shader file : {} does not exist", path);
        return false;
    }

    return true;
}

std::string ShaderProgram::get_cross_platform_path(const std::string& folderName, const std::string& fileName) {
    // TODO function on renderer to check current backend once multiple graphics APIs are supported

    std::string shaderPath = "SHADER_BINARY_NOT_SET";
    shaderPath = "../res/shaders/";
    shaderPath.append(folderName);
    shaderPath.append("/");
    shaderPath.append(fileName);

    return shaderPath;
}
void ShaderProgram::generate_default_asset() {}
void ShaderProgram::check_compile_errors(GLuint shader, uint16_t type) {
    int status = 0;
    std::string shaderHintType = "";
    if (type == GL_VERTEX_SHADER) {
        shaderHintType = "Vertex";
    } else if (type == GL_FRAGMENT_SHADER) {
        shaderHintType = "Fragment";
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string errorLog(length, ' ');
        glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
        log::debug("{} shader failed to compile - ID : {} - name : {}\n{}", shaderHintType, m_id, m_assetName,
                   errorLog);
    }
}

void ShaderProgram::check_link_errors() {
    int status = 0;

    glGetProgramiv(m_id, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);

        std::string errorLog(length, ' ');
        glGetProgramInfoLog(m_id, length, &length, &errorLog[0]);
        log::error("Program link failed - ID : {} - name : {}\n{}", m_id, m_assetName, errorLog);
    } else {
        log::debug("Program link success - ID : {} - name : {} ", m_id, m_assetName);
    }
}

}  // namespace components
}  // namespace beet
