/*
** EPITECH PROJECT, 2026
** gui/src/Resources/TextureManager
** File description:
** TextureManager resource header file
*/

#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>

/* @brief TextureManager Class */
class TextureManager
{
public:
    /**
     * @brief Load a texture from a file, or return the cached one.
     *
     * @param key      The identifier used to retrieve the texture later
     * @param path     The path to the image file on disk
     *
     * @return A reference to the loaded (or cached) texture.
     */
    sf::Texture &load(const std::string &key, const std::string &path);

    /**
     * @brief Get a previously loaded texture.
     *
     * @param key      The identifier the texture was loaded with
     *
     * @return A reference to the texture.
     */
    sf::Texture &get(const std::string &key);

    /**
     * @brief Tell whether a texture is registered under a key.
     *
     * @param key      The identifier to look up
     *
     * @return true if a texture exists for the key, false otherwise.
     */
    bool has(const std::string &key) const;

private:
    std::unordered_map<std::string, sf::Texture> _textures;
};
