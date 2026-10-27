/*
** EPITECH PROJECT, 2026
** Inventory.cpp
** File description:
** Parser turning the raw Inventory response into a typed structure
*/

#include "Inventory.hpp"

#include <sstream>

namespace ai {

namespace {

std::string stripBrackets(const std::string &response)
{
    const std::size_t open = response.find('[');
    const std::size_t close = response.rfind(']');

    if (open == std::string::npos || close == std::string::npos || close < open) {
        return response;
    }
    return response.substr(open + 1, close - open - 1);
}

} // namespace

int Inventory::get(ObjectType type) const
{
    switch (type) {
    case ObjectType::Food:
        return _food;
    case ObjectType::Linemate:
        return _linemate;
    case ObjectType::Deraumere:
        return _deraumere;
    case ObjectType::Sibur:
        return _sibur;
    case ObjectType::Mendiane:
        return _mendiane;
    case ObjectType::Phiras:
        return _phiras;
    case ObjectType::Thystame:
        return _thystame;
    default:
        return 0;
    }
}

void Inventory::set(ObjectType type, int value)
{
    switch (type) {
    case ObjectType::Food:
        _food = value;
        break;
    case ObjectType::Linemate:
        _linemate = value;
        break;
    case ObjectType::Deraumere:
        _deraumere = value;
        break;
    case ObjectType::Sibur:
        _sibur = value;
        break;
    case ObjectType::Mendiane:
        _mendiane = value;
        break;
    case ObjectType::Phiras:
        _phiras = value;
        break;
    case ObjectType::Thystame:
        _thystame = value;
        break;
    default:
        break;
    }
}

Inventory parseInventory(const std::string &response)
{
    Inventory inventory;
    std::istringstream fields(stripBrackets(response));
    std::string field;

    // Each field is a "name count" pair separated by commas.
    while (std::getline(fields, field, ',')) {
        std::istringstream pair(field);
        std::string name;
        int count = 0;

        if (pair >> name >> count) {
            inventory.set(objectTypeFromString(name), count);
        }
    }
    return inventory;
}

} // namespace ai
