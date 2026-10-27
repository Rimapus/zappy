/*
** EPITECH PROJECT, 2026
** gui/src/Resources/FontManager
** File description:
** FontManager source file
*/

#include "FontManager.hpp"
#include "AssetPath.hpp"
#include <stdexcept>

/**
 * @brief Load a font from a file, or return the cached one.
 *
 * @param key      The identifier used to retrieve the font later
 * @param path     The path to the font file on disk
 *
 * @return A reference to the loaded (or cached) font.
 */
sf::Font &FontManager::load(const std::string &key, const std::string &path)
{
    auto it = _fonts.find(key);

    if (it != _fonts.end()) {
        return it->second;
    }

    sf::Font &font = _fonts[key];
    std::string resolved = AssetPath::resolve(path);

    if (font.openFromFile(resolved) == false) {
        _fonts.erase(key);
        throw std::runtime_error("FontManager: cannot load font: " + resolved);
    }
    font.setSmooth(false);
    return font;
}

/**
 * @brief Get a previously loaded font.
 *
 * @param key      The identifier the font was loaded with
 *
 * @return A reference to the font.
 */
sf::Font &FontManager::get(const std::string &key)
{
    return _fonts.at(key);
}

/**
 * @brief Tell whether a font is registered under a key.
 *
 * @param key      The identifier to look up
 *
 * @return true if a font exists for the key, false otherwise.
 */
bool FontManager::has(const std::string &key) const
{
    return _fonts.find(key) != _fonts.end();
}
