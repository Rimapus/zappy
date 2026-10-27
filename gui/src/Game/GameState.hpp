/*
 * EPITECH PROJECT, 2026
 * GameState.hpp
 * File description:
 * gamestate
 */

#pragma once

#include "../ECS/ECSConfig.hpp"
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/* @brief A single broadcast message displayed in the chat overlay */
struct ChatMessage
{
    std::uint32_t _playerId = 0;
    std::string _team;
    std::string _text;
    float _age = 0.0f;
    bool _isServer = false;
};

/* @brief Global game state stored as an ECS resource */
struct GameState
{
    std::uint32_t _mapWidth = 0;
    std::uint32_t _mapHeight = 0;
    std::uint32_t _freq = 0;
    float _musicVolume = 50.0f;
    int _tileOriginX = 0;
    int _tileOriginY = 0;
    bool _gameOver = false;
    std::string _winnerTeam;
    bool _disconnected = false;
    bool _tileSelected = false;
    std::uint32_t _selectedTileX = 0;
    std::uint32_t _selectedTileY = 0;
    bool _tileHovered = false;
    std::uint32_t _hoveredTileX = 0;
    std::uint32_t _hoveredTileY = 0;
    bool _playerSelected = false;
    std::uint32_t _selectedPlayerId = 0;
    bool _playerHovered = false;
    std::uint32_t _hoveredPlayerId = 0;
    std::vector<std::string> _teams;
    std::unordered_map<std::uint32_t, Entity> _players;
    std::unordered_map<std::uint32_t, Entity> _eggs;
    std::map<std::pair<std::uint32_t, std::uint32_t>, Entity> _tiles;
    std::map<std::pair<std::uint32_t, std::uint32_t>, float> _tileLift;
    std::map<std::pair<std::uint32_t, std::uint32_t>, std::vector<std::uint32_t>> _incantations;
    std::deque<ChatMessage> _chat;
};
