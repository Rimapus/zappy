/*
** EPITECH PROJECT, 2026
** gui/src/Resources/TextureManager
** File description:
** TextureManager source file
*/

#include "TextureManager.hpp"
#include "AssetPath.hpp"
#include <stdexcept>

/**
 * @brief Load a texture from a file, or return the cached one.
 *
 * @param key      The identifier used to retrieve the texture later
 * @param path     The path to the image file on disk
 *
 * @return A reference to the loaded (or cached) texture.
 */
sf::Texture &TextureManager::load(const std::string &key, const std::string &path)
{
    auto it = _textures.find(key);

    if (it != _textures.end()) {
        return it->second;
    }

    sf::Texture &texture = _textures[key];
    std::string resolved = AssetPath::resolve(path);

    if (texture.loadFromFile(resolved) == false) {
        _textures.erase(key);
        throw std::runtime_error("TextureManager: cannot load texture: " + resolved);
    }
    return texture;
}

/**
 * @brief Get a previously loaded texture.
 *
 * @param key      The identifier the texture was loaded with
 *
 * @return A reference to the texture.
 */
sf::Texture &TextureManager::get(const std::string &key)
{
    return _textures.at(key);
}

/**
 * @brief Tell whether a texture is registered under a key.
 *
 * @param key      The identifier to look up
 *
 * @return true if a texture exists for the key, false otherwise.
 */
bool TextureManager::has(const std::string &key) const
{
    return _textures.find(key) != _textures.end();
}
