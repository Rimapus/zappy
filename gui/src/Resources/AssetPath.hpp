/*
** EPITECH PROJECT, 2026
** gui/src/Resources/AssetPath
** File description:
** Resolve asset paths relative to the executable root
*/

#pragma once

#include <string>

/* @brief Asset path resolution helpers */
namespace AssetPath {

/**
 * @brief Resolve a relative asset path against the executable's directory.
 *
 * @param relativePath     The asset path relative to the executable root
 *
 * @return The resolved path to the asset on disk.
 */
std::string resolve(const std::string &relativePath);

} // namespace AssetPath
