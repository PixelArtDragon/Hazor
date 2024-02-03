#version 450 core

in vec3 interNormal;

vec3 lightDir = normalize(vec3(0.0f, 0.0f, -1.0f));

void main() {
    vec3 normal = normalize(interNormal);
    float diff = max(dot(normal, -lightDir), 0.0f);
    gl_FragColor = vec4(vec3(1.0f, 1.0f, 1.0f) * (diff + 0.1), 1.0f);
}
