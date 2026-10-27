/*
** EPITECH PROJECT, 2026
** Data.hpp
** File description:
** Data
*/

#pragma once

#include "GameEvent.hpp"
#include "Gui.hpp"
#include "PeriodicEvents.hpp"
#include "communication/Socket.hpp"
#include "map/Map.hpp"
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace server {

class Client;

class ClientsData
{
public:
    std::vector<std::shared_ptr<Client>> _pendingClients;
    std::vector<std::shared_ptr<Client>> _activeClients;
};

class ServerData
{
public:
    ServerData(std::unique_ptr<Socket> serverSocket, std::vector<std::string> teams, std::size_t frequency)
        : _serverSocket(std::move(serverSocket)), _teams(std::move(teams)), _frequency(frequency) {};

    auto &getClientsData(void) { return _clientsData; };

    auto &getClientsData(void) const { return _clientsData; };

    auto &getGameEvents(void) { return _gameEvents; };

    auto &getGameEvents(void) const { return _gameEvents; };

    auto &getServerSocket(void) const { return _serverSocket; }

    auto &getTeams(void) { return _teams; }

    auto &getTeams(void) const { return _teams; }

    void setMap(std::size_t width, std::size_t height) { _map = Map(width, height); }

    auto &getMap(void) { return _map; }

    auto &getMap(void) const { return _map; }

    auto getFrequency(void) const { return _frequency; }

    void setFrequency(std::size_t newF);

    auto &getPeriodicEvents(void) { return _periodicEvents; }

    auto &getPeriodicEvents(void) const { return _periodicEvents; }

    auto &getGui(void) { return _gui; }

    auto &getGui(void) const { return _gui; }

    auto getFinished(void) const { return _gameFinished; }

    void setFinished(bool finished) { _gameFinished = finished; }

private:
    std::shared_ptr<Socket> _serverSocket;
    std::vector<std::string> _teams;
    ClientsData _clientsData;
    std::priority_queue<GameEvent, std::vector<GameEvent>, CompareEventTime> _gameEvents;
    Map _map;
    std::size_t _frequency;
    PeriodicEventsData _periodicEvents;
    Gui _gui;
    bool _gameFinished = false;
};

}
