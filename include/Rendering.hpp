#pragma once
#include "Mesh.hpp"
#include "RenderingHandles.hpp"
#include "Scene.hpp"
#include "Util.hpp"
#include "Window.hpp"
#include "rendering_internals/ElementBuffer.hpp"
#include "rendering_internals/Framebuffer.hpp"
#include "rendering_internals/GPUMesh.hpp"
#include "rendering_internals/Shader.hpp"
#include "rendering_internals/VertexArray.hpp"
#include "rendering_internals/VertexBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <unordered_map>

namespace tel {
template <typename T>
class ResourceLookup {
  public:
    using Handle = unsigned int;

    Handle add(T resource) {
        Handle currentHandle = nextHandle;
        lookup.emplace(currentHandle, std::move(resource));
        ++nextHandle;
        return currentHandle;
    }

    template <typename Key>
    [[nodiscard]] T* find(const Key& key) {
        auto iter = lookup.find(key);
        if (iter != lookup.end()) {
            return &iter->second;
        }
        return nullptr;
    }

  private:
    std::unordered_map<Handle, T> lookup;
    Handle nextHandle = 0;
};

template <typename T>
constexpr GLenum gl_enum();

template <>
constexpr GLenum gl_enum<float>() {
    return GL_FLOAT;
}

template <>
constexpr GLenum gl_enum<unsigned int>() {
    return GL_UNSIGNED_INT;
}

class Rendering {
  public:
    explicit Rendering(Window* window) : window(window) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, nullptr);
        glEnable(GL_DEPTH_TEST);
    }

    void render_scene(const Scene& scene) {
        begin_frame(window->default_framebuffer());
        for (const auto& object : scene.sceneObjects) {
            const auto meshObject = lookups.get_lookup<GPUMesh>().find(object.renderable.mesh);
            const auto shader = lookups.get_lookup<Program>().find(object.renderable.shader);
            bind(*shader);
            assert(meshObject);
            assert(shader);
            set_uniform(*shader, "model", object.transform);
            set_uniform(*shader, "camera", scene.camera.matrix());
            draw(*meshObject);
        }
    }

    MeshHandle load_mesh(const Mesh& mesh) {
        GPUMesh meshObject = create_with_buffers();
        meshObject.numIndices = mesh.triangles.size();
        stream(meshObject.elementBuffer, mesh.triangles);
        stream(meshObject, mesh);
        return lookups.get_lookup<GPUMesh>().add(std::move(meshObject));
    }

    std::expected<ShaderHandle, ShaderCompilationError> load_shader(const std::string_view& vertexCode,
                                                                    const std::string_view& fragmentCode,
                                                                    const ProgramOptions& options = {}) {
        auto vertexShader = Shader<ShaderType::Vertex>::create(vertexCode);
        if (!vertexShader.has_value()) {
            return std::unexpected(vertexShader.error());
        }
        auto fragmentShader = Shader<ShaderType::Fragment>::create(fragmentCode);
        if (!fragmentShader.has_value()) {
            return std::unexpected(fragmentShader.error());
        }
        auto program = Program::create(vertexShader.value(), fragmentShader.value(), options);
        if (program.has_value()) {
            return lookups.get_lookup<Program>().add(std::move(program.value()));
        }
        return std::unexpected(program.error());
    }

  private:
    struct CurrentlyBound {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint fbo = 0;
        GLuint texture = 0;
        GLuint program = 0;
    } currentlyBound;

    template <typename... ResourceTypes>
    class Lookups {
      public:
        template <typename T>
        ResourceLookup<T>& get_lookup() {
            return std::get<ResourceLookup<T>>(lookups);
        }

      private:
        std::tuple<ResourceLookup<ResourceTypes>...> lookups;
    };

    Lookups<GPUMesh, Program> lookups;
    Window* window;

    template <typename T>
    void stream(const VertexBuffer<T>& buffer, std::span<const T> data) {
        if (data.empty()) {
            std::cout << "Empty buffer!\n";
            return;
        }
        bind(buffer);
        glBufferStorage(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), 0);
    }

    void stream(const GPUMesh& gpuMesh, const Mesh& mesh) {
        auto func = [&]<size_t... indices>(std::index_sequence<indices...>) {
            (stream(std::get<indices>(gpuMesh.attachments), std::span(std::get<indices>(mesh.vertex_attributes()))), ...);
        };
        func(std::make_index_sequence<std::tuple_size_v<decltype(gpuMesh.attachments)>>{});
    }

    void stream(const ElementBuffer& elementBuffer, std::span<const ElementBuffer::Index> data) {
        if (data.empty()) {
            return;
        }
        bind(elementBuffer);
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(data.size_bytes()), data.data(), 0);
    }

    static void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                          const GLchar* message, const void* userParam) {
        std::cout << message << std::endl;
    }

    static void begin_frame(const Framebuffer& frameBuffer) {
        glViewport(0, 0, frameBuffer.width(), frameBuffer.height());
        glClearColor(0.3f, 0.3f, 0.5f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void draw(const GPUMesh& meshObject) {
        bind(meshObject.vertexArray);
        glDrawElements(GL_TRIANGLES, static_cast<GLint>(meshObject.numIndices), gl_enum<ElementBuffer::Index>(),
                       nullptr);
    }

    void bind(const VertexArray& vertexArray) {
        if (currentlyBound.vao != vertexArray.underlying()) {
            glBindVertexArray(vertexArray.underlying());
            currentlyBound.vao = vertexArray.underlying();
        }
    }

    void bind(const ElementBuffer& elementBuffer) {
        if (currentlyBound.ebo != elementBuffer.underlying()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.underlying());
            currentlyBound.ebo = elementBuffer.underlying();
        }
    }

    void bind(const Program& program) {
        if (currentlyBound.program != program.underlying()) {
            glUseProgram(program.underlying());
            currentlyBound.program = program.underlying();
        }
    }

    void force_binding(const VertexArray& vertexArray) {
        glBindVertexArray(vertexArray.underlying());
        currentlyBound.vao = vertexArray.underlying();
    }

    template <typename T>
    void force_binding(const VertexBuffer<T>& vertexBuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.underlying());
        currentlyBound.vbo = vertexBuffer.underlying();
    }

    void force_binding(const Program& program) {
        glUseProgram(program.underlying());
        currentlyBound.program = program.underlying();
    }

    template <typename T>
    void set_uniform(const Program& program, std::string_view name, const T& value) {
        bind(program);
        const auto location = program.get_uniform_location(name);
        if (!location) {
            return;
        }
        if constexpr (std::is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(location.value(), 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void force_binding(const ElementBuffer& elementBuffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.underlying());
        currentlyBound.ebo = elementBuffer.underlying();
    }

    template <typename T>
    void bind(const VertexBuffer<T>& buffer) {
        if (currentlyBound.vbo != buffer.underlying()) {
            glBindBuffer(GL_ARRAY_BUFFER, buffer.underlying());
            currentlyBound.vbo = buffer.underlying();
        }
    }

    GPUMesh create_with_buffers() {
        GPUMesh mesh{};
        mesh.vertexArray = VertexArray::create();
        mesh.elementBuffer = ElementBuffer::create();
        bind(mesh.vertexArray);
        for_each(mesh.attachments, []<typename T>(T& attachment) { attachment = T::create(); });
        link_attachments(mesh);
        return mesh;
    }



    template<size_t index, typename Attachment>
    void create_attribute(const VertexBuffer<Attachment>& buffer) {
        bind(buffer);
        glVertexAttribPointer(index, 1, gl_enum<Attachment>(), false, sizeof(Attachment), nullptr);
        glEnableVertexAttribArray(index);
    }

    template<size_t index, typename T, int length>
    void create_attribute(const VertexBuffer<glm::vec<length, T>>& buffer) {
        bind(buffer);
        glVertexAttribPointer(index, length, gl_enum<T>(), false, sizeof(glm::vec<length, T>), nullptr);
        glEnableVertexAttribArray(index);
    }

    void link_attachments(GPUMesh& mesh) {
        bind(mesh.vertexArray);
        for_each_i(mesh.attachments, [&]<size_t i>(auto& vertexBuffer) {
            create_attribute<i>(vertexBuffer);
        });
    }
};
} // namespace tel
