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
        auto positionBuffer = VertexBuffer<glm::vec3>::create();
        auto normalBuffer = VertexBuffer<glm::vec3>::create();
        auto triangleBuffer = ElementBuffer::create();
        GPUMesh meshObject{};
        meshObject.numIndices = mesh.triangles.size();
        meshObject.vertexArray = VertexArray::create();

        link_element_array(meshObject, std::move(triangleBuffer));
        link_attachments(meshObject, std::move(positionBuffer), std::move(normalBuffer));
        stream(meshObject, mesh);
        stream(meshObject.elementBuffer, mesh.triangles);
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
        // using Indices = std::index_sequence_for<decltype(GPUMesh::attachments)>;
        auto func = [&]<size_t index>(this auto self) {
            using Attachment =
                typename std::remove_reference_t<std::tuple_element_t<index, decltype(gpuMesh.attachments)>>::Type;
            stream<Attachment>(std::get<index>(gpuMesh.attachments), std::get<index>(mesh.vertex_attributes()));
            if constexpr (index + 1 < std::tuple_size_v<decltype(gpuMesh.attachments)>) {
                self.template operator()<index + 1>();
            }
        };
        func.operator()<0>();
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

    void link_element_array(GPUMesh& meshObject, ElementBuffer&& elementBuffer) {
        force_binding(meshObject.vertexArray);
        force_binding(elementBuffer);
        meshObject.elementBuffer = std::move(elementBuffer);
    }

    template <typename... Attachments>
    void link_attachments(GPUMesh& meshObject, VertexBuffer<Attachments>&&... buffers) {
        assert(meshObject.vertexArray.underlying() != 0);
        bind(meshObject.vertexArray);
        decltype(auto) buffers_tuple = std::tie(buffers...);

        auto func = [&]<int index>(this auto self) -> void {
            using Attachment =
                typename std::remove_reference_t<std::tuple_element_t<index, decltype(buffers_tuple)>>::Type;
            auto& buffer = std::get<index>(buffers_tuple);
            bind(buffer);
            glVertexAttribPointer(index, Attachment::length(), gl_enum<typename Attachment::value_type>(), false,
                                  sizeof(Attachment), nullptr);
            glEnableVertexAttribArray(index);
            std::get<index>(meshObject.attachments) = std::move(buffer);
            if constexpr (index + 1 < sizeof...(buffers)) {
                self.template operator()<index + 1>();
            }
        };

        func.template operator()<0>();
    }
};
} // namespace tel
