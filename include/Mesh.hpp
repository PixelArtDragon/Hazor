#pragma once
#include "Util.hpp"

#include <functional>
#include <glm/vec3.hpp>
#include <ranges>
#include <vector>

namespace tel {
using Position = glm::vec3;

using Normal = glm::vec3;

using TriangleIndex = unsigned int;

struct Mesh {
    std::vector<Position> positions;
    std::vector<Normal> normals;
    std::vector<TriangleIndex> triangles;

    [[nodiscard]] auto vertex_attributes() const { return std::tie(positions, normals); }

    void reserve(auto capacity) {
        // I don't know why this doesn't work with the "apply to tuple of attributes", it worked fine in the const
        // version
        positions.reserve(capacity);
        normals.reserve(capacity);
        //  There will be at least as many triangle indices as there are vertices, so reserve extra
        triangles.reserve(capacity * 3 / 2);
    }
};

inline [[nodiscard]] Mesh join(Mesh first, Mesh second) {
    Mesh combined = std::move(first);
    unsigned int triangleOffset = first.positions.size();
    combined.positions.append_range(second.positions);
    combined.normals.append_range(second.normals);
    combined.triangles.append_range(second.triangles |
                                    std::views::transform(std::bind_front(std::plus{}, triangleOffset)));
    return combined;
}

[[nodiscard]] constexpr bool mesh_is_valid(const Mesh& mesh) {
    if (mesh.triangles.empty()) {
        return mesh.positions.empty();
    }
    const TriangleIndex maxIndex = std::ranges::max(mesh.triangles);
    return all_of([&](const auto& vec) { return vec.size() > maxIndex; }, mesh.vertex_attributes());
}
} // namespace tel