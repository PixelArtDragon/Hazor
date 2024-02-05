#pragma once
#include "Mesh.hpp"

#include <expected>

namespace tel {
struct MeshLoadError {};

std::expected<Mesh, MeshLoadError> load_mesh_from_memory(std::string_view data);
} // namespace tel