/*
** EPITECH PROJECT, 2026
** gui/src/Resources/AssetPath
** File description:
** Resolve asset paths relative to the executable root
*/

#include "AssetPath.hpp"
#include <cstdint>
#include <filesystem>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <vector>
#endif

/**
 * @brief Read the absolute path of the running executable.
 *
 * @return The executable path, or an empty path when it cannot be determined.
 */
static std::filesystem::path executablePath()
{
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);

    if (length == 0 || length == MAX_PATH) {
        return {};
    }
    return std::filesystem::path(buffer, buffer + length);
#elif defined(__APPLE__)
    std::uint32_t size = 0;

    _NSGetExecutablePath(nullptr, &size);
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        return {};
    }
    return std::filesystem::path(buffer.data());
#else
    std::error_code error;
    std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", error);

    if (error) {
        return {};
    }
    return exe;
#endif
}

/**
 * @brief Find the directory of the running executable.
 *
 * @return The executable's directory, or an empty path on failure.
 */
static std::filesystem::path executableDir()
{
    std::filesystem::path exe = executablePath();
    std::error_code error;
    std::filesystem::path canonical = std::filesystem::weakly_canonical(exe, error);

    if (exe.empty() || error) {
        return {};
    }
    return canonical.parent_path();
}

/**
 * @brief Resolve a relative asset path against the executable's directory.
 *
 * @param relativePath     The asset path relative to the executable root
 *
 * @return The resolved path to the asset on disk.
 */
std::string AssetPath::resolve(const std::string &relativePath)
{
    std::filesystem::path path(relativePath);

    if (path.is_absolute()) {
        return relativePath;
    }

    std::filesystem::path root = executableDir();

    if (root.empty()) {
        return relativePath;
    }
    return (root / path).string();
}
