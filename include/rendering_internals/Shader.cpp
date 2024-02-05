#include "Shader.hpp"

std::vector<tel::Program::ProgramVariable> tel::Program::extract_attributes(GLuint program) {
    std::vector<ProgramVariable> variables;
    GLint maximumAttributeName{};
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maximumAttributeName);
    GLint numAttributes{};
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    variables.reserve(numAttributes);
    for (int i = 0; i < numAttributes; ++i) {
        ProgramVariable variable;
        variable.location.name.resize(maximumAttributeName);
        GLint actualVariableNameLength{};
        glGetActiveAttrib(program, i, maximumAttributeName, &actualVariableNameLength, &variable.variableSize,
                          &variable.type, variable.location.name.data());
        variable.location.name.resize(actualVariableNameLength);
        variable.location.index = glGetAttribLocation(program, variable.location.name.c_str());
        variables.emplace_back(variable);
    }
    return variables;
}

std::vector<tel::Program::ProgramVariable> tel::Program::extract_uniforms(GLuint program) {
    std::vector<ProgramVariable> uniforms;
    GLint maximumAttributeName{};
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maximumAttributeName);
    GLint numUniforms{};
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
    uniforms.reserve(numUniforms);
    for (int i = 0; i < numUniforms; ++i) {
        ProgramVariable uniform;
        uniform.location.name.resize(maximumAttributeName);
        uniform.location.index = i;
        GLint actualVariableNameLength{};
        glGetActiveUniform(program, i, maximumAttributeName, &actualVariableNameLength, &uniform.variableSize,
                           &uniform.type, uniform.location.name.data());
        uniform.location.name.resize(actualVariableNameLength);
        uniform.location.index = glGetUniformLocation(program, uniform.location.name.c_str());
        uniforms.emplace_back(uniform);
    }
    return uniforms;
}
