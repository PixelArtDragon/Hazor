#include "MeshLoading.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <ranges>

inline glm::vec3 from_assimp(const aiVector3D& vector3) { return {vector3.x, vector3.y, vector3.z}; }

inline tel::Mesh convert_to_mesh(const aiMesh& assimp_mesh) {
    tel::Mesh mesh;
    mesh.reserve(assimp_mesh.mNumVertices);
    mesh.positions.append_range(std::span(assimp_mesh.mVertices, assimp_mesh.mNumVertices) |
                                std::views::transform(from_assimp));
    mesh.normals.append_range(std::span(assimp_mesh.mNormals, assimp_mesh.mNumVertices) |
                              std::views::transform(from_assimp));
    for (const aiFace& face : std::span(assimp_mesh.mFaces, assimp_mesh.mNumFaces)) {
        mesh.triangles.append_range(std::span(face.mIndices, face.mNumIndices));
    }
    return mesh;
}

std::expected<tel::Mesh, tel::MeshLoadError> tel::load_mesh_from_memory(std::string_view data) {
    Assimp::Importer importer;
    const auto* scene =
        importer.ReadFileFromMemory(data.data(), data.size(), aiProcess_OptimizeMeshes | aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return std::unexpected(MeshLoadError{});
    }
    return std::ranges::fold_left_first(
               std::span(scene->mMeshes, scene->mNumMeshes) |
                   std::views::transform([](const auto* ptr) { return convert_to_mesh(*ptr); }),
               join)
        .value_or(Mesh{});
}