#pragma once
#include <expected>
#include <filesystem>

namespace tel {
enum class FileLoadError : std::remove_reference_t<decltype(errno)> {

};
std::expected<std::string, FileLoadError> load_file(const std::filesystem::path& file);
} // namespace tel