#include "FileLoading.hpp"

#include <fstream>

std::expected<std::string, tel::FileLoadError> tel::load_file(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::in | std::ios::binary);
    if (not in) {
        return std::unexpected(static_cast<FileLoadError>(errno));
    }

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(contents.data(), std::ssize(contents));
    in.close();
    return contents;
}