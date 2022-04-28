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

void ShaderProgram::on_destroy() {
    glDeleteProgram(m_program);
    m_UniformLocations.clear();
}

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

GLuint ShaderProgram::create_shader(const std::string& path, uint16_t glShaderType) {
    GLuint vs = 0;
    std::string vsString = load_shader_text(path);
    const GLchar* vsSourcePtr = vsString.c_str();

    vs = glCreateShader(glShaderType);
    glShaderSource(vs, 1, &vsSourcePtr, nullptr);
    glCompileShader(vs);

    check_compile_errors(vs, glShaderType);
    return vs;
}

void ShaderProgram::create_program(std::string& vertexShaderPath, std::string& fragmentShaderPath) {
    GLuint vs = create_shader(vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fs = create_shader(fragmentShaderPath, GL_FRAGMENT_SHADER);

    m_program = glCreateProgram();

    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);

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
void ShaderProgram::check_compile_errors(GLuint shader, uint16_t glShaderType) {
    int status = 0;
    std::string type = "";
    if (glShaderType == GL_VERTEX_SHADER) {
        type = "Vertex";
    } else if (glShaderType == GL_FRAGMENT_SHADER) {
        type = "Fragment";
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string errorLog(length, ' ');
        glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
        log::debug("{} shader failed to compile - ID : {} - name : {}\n{}", type, m_program, m_assetName, errorLog);
    }
}

void ShaderProgram::check_link_errors() {
    int status = 0;

    glGetProgramiv(m_program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

        std::string errorLog(length, ' ');
        glGetProgramInfoLog(m_program, length, &length, &errorLog[0]);
        log::error("Program link failed - ID : {} - name : {}\n{}", m_program, m_assetName, errorLog);
    } else {
        log::debug("Program link success - ID : {} - name : {} ", m_program, m_assetName);
    }
}

}  // namespace components
}  // namespace beet
