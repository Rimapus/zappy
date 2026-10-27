/*
** EPITECH PROJECT, 2026
** gui/src/Resources/FontManager
** File description:
** FontManager resource header file
*/

#pragma once

#include <SFML/Graphics/Font.hpp>
#include <string>
#include <unordered_map>

/* @brief FontManager Class */
class FontManager
{
public:
    /**
     * @brief Load a font from a file, or return the cached one.
     *
     * @param key      The identifier used to retrieve the font later
     * @param path     The path to the font file on disk
     *
     * @return A reference to the loaded (or cached) font.
     */
    sf::Font &load(const std::string &key, const std::string &path);

    /**
     * @brief Get a previously loaded font.
     *
     * @param key      The identifier the font was loaded with
     *
     * @return A reference to the font.
     */
    sf::Font &get(const std::string &key);

    /**
     * @brief Tell whether a font is registered under a key.
     *
     * @param key      The identifier to look up
     *
     * @return true if a font exists for the key, false otherwise.
     */
    bool has(const std::string &key) const;

private:
    std::unordered_map<std::string, sf::Font> _fonts;
};
