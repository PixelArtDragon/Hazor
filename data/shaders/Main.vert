#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 interNormal;

uniform mat4 model;
uniform mat4 camera;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0);
    interNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = camera * worldPosition;
}
