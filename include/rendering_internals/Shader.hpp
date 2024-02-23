#pragma once

#include <GL/glew.h>
#include <expected>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "Moving.hpp"

namespace tel {
enum class ShaderType { Vertex, Fragment };

struct ShaderCompilationError {
    std::vector<GLchar> text;
};

template <ShaderType type>
class Shader {
  public:
    static std::expected<Shader, ShaderCompilationError> create(const std::string_view& code) {
        const GLuint shader = glCreateShader(shader_type());
        const char* str = code.data();
        const auto length = static_cast<GLint>(code.length());
        glShaderSource(shader, 1, &str, &length);
        glCompileShader(shader);
        GLint success{};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success) {
            return Shader(shader);
        }
        GLint logSize{};
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        ShaderCompilationError error{};
        error.text.resize(logSize);
        glGetShaderInfoLog(shader, logSize, &logSize, error.text.data());
        glDeleteShader(shader);
        return std::unexpected(error);
    }

    Shader(const Shader& other) = delete;

    Shader(Shader&& other) noexcept = default;

    Shader& operator=(const Shader& other) = delete;

    Shader& operator=(Shader&& other) = default;

    ~Shader() { glDeleteShader(shader); }

    [[nodiscard]] GLuint underlying() const { return shader; }

  private:
    explicit Shader(GLuint shader) : shader(shader) {}

    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> shader;

    constexpr static GLenum shader_type() {
        switch (type) {
        case ShaderType::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderType::Fragment:
            return GL_FRAGMENT_SHADER;
        default:
            std::unreachable();
        }
    }
};

struct ProgramLocation {
    GLuint index{};
    std::string name{};
};

/*
struct OutputBuffers {};
*/

struct ProgramOptions {
    std::vector<ProgramLocation> attributeLocations;
};

class Program {
  public:
    Program(const Program& other) = delete;

    Program(Program&& other) noexcept = default;
    Program& operator=(const Program& other) = delete;

    Program& operator=(Program&& other) noexcept = default;

    static std::expected<Program, ShaderCompilationError> create(const Shader<ShaderType::Vertex>& vertexShader,
                                                                 const Shader<ShaderType::Fragment>& fragmentShader,
                                                                 const ProgramOptions& programOptions) {
        const GLuint program = glCreateProgram();
        for (const auto& [index, name] : programOptions.attributeLocations) {
            glBindAttribLocation(program, index, name.c_str());
        }
        glAttachShader(program, vertexShader.underlying());
        glAttachShader(program, fragmentShader.underlying());
        glLinkProgram(program);
        GLint isLinked{};
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked) {
            return Program(program, extract_details(program));
        }
        return std::unexpected(ShaderCompilationError{});
    }

    [[nodiscard]] GLuint underlying() const { return program; }

    [[nodiscard]] std::optional<int> get_uniform_location(std::string_view name) const {
        const auto iter =
            std::ranges::find(details.uniformLocations, name, [](const auto& uniformLocation) -> std::string_view {
                return uniformLocation.location.name;
            });
        if (iter == details.uniformLocations.end()) {
            return std::nullopt;
        }
        return iter->location.index;
    }

    ~Program() { glDeleteProgram(program); }

  private:
    struct ProgramVariable {
        ProgramLocation location{};
        GLenum type{};
        GLint variableSize{};
    };

    struct ProgramDetails {
        std::vector<ProgramVariable> attributeLocations;
        std::vector<ProgramVariable> uniformLocations;
    };

    Moving<GLuint, 0, EngagedMoveAssignBehavior::Assert> program;
    ProgramDetails details;

    static std::vector<ProgramVariable> extract_attributes(GLuint program);
    static std::vector<ProgramVariable> extract_uniforms(GLuint program);

    static ProgramDetails extract_details(GLuint program) {
        ProgramDetails details{};
        details.attributeLocations = extract_attributes(program);
        details.uniformLocations = extract_uniforms(program);
        return details;
    }

    Program(GLuint program, ProgramDetails details) : program(program), details(std::move(details)) {}
};

} // namespace tel
