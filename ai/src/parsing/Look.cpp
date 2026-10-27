/*
** EPITECH PROJECT, 2026
** Look.cpp
** File description:
** Parser turning the raw Look response into an exploitable tile vector
*/

#include "Look.hpp"

#include <algorithm>
#include <array>
#include <sstream>
#include <utility>

namespace ai {

namespace {

/// Mapping between server token and ObjectType, used both ways.
constexpr std::array<std::pair<const char *, ObjectType>, 8> OBJECT_NAMES{
    {
     {"player", ObjectType::Player},
     {"food", ObjectType::Food},
     {"linemate", ObjectType::Linemate},
     {"deraumere", ObjectType::Deraumere},
     {"sibur", ObjectType::Sibur},
     {"mendiane", ObjectType::Mendiane},
     {"phiras", ObjectType::Phiras},
     {"thystame", ObjectType::Thystame},
     }
};

/// Extract the content between the first '[' and the last ']'.
/// If no brackets are found the whole string is returned unchanged.
std::string stripBrackets(const std::string &response)
{
    const std::size_t open = response.find('[');
    const std::size_t close = response.rfind(']');

    if (open == std::string::npos || close == std::string::npos || close < open) {
        return response;
    }
    return response.substr(open + 1, close - open - 1);
}

/// Split @p text on @p delimiter, keeping empty fields (needed for empty tiles).
std::vector<std::string> split(const std::string &text, char delimiter)
{
    std::vector<std::string> parts;
    std::string field;
    std::istringstream stream(text);

    while (std::getline(stream, field, delimiter)) {
        parts.push_back(field);
    }
    // getline yields nothing for an empty string and drops a trailing empty
    // field after the last delimiter; restore them so the field count is always
    // (number of delimiters + 1).
    if (text.empty() || text.back() == delimiter) {
        parts.emplace_back();
    }
    return parts;
}

/// Build a Tile from the whitespace-separated objects of one tile field.
Tile parseTile(const std::string &field)
{
    Tile tile;
    std::istringstream stream(field);
    std::string token;

    while (stream >> token) {
        tile._objects.push_back(objectTypeFromString(token));
    }
    return tile;
}

} // namespace

int Tile::count(ObjectType type) const
{
    return static_cast<int>(std::count(_objects.begin(), _objects.end(), type));
}

bool Tile::contains(ObjectType type) const
{
    return std::find(_objects.begin(), _objects.end(), type) != _objects.end();
}

ObjectType objectTypeFromString(const std::string &name)
{
    for (const auto &[token, type] : OBJECT_NAMES) {
        if (name == token) {
            return type;
        }
    }
    return ObjectType::Unknown;
}

std::string objectTypeToString(ObjectType type)
{
    for (const auto &[token, value] : OBJECT_NAMES) {
        if (value == type) {
            return token;
        }
    }
    return "unknown";
}

std::vector<Tile> parseLook(const std::string &response)
{
    const std::string content = stripBrackets(response);
    std::vector<Tile> tiles;

    for (const std::string &field : split(content, ',')) {
        tiles.push_back(parseTile(field));
    }
    return tiles;
}

} // namespace ai
