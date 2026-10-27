/*
** EPITECH PROJECT, 2026
** Client.hpp
** File description:
** Client
*/

#pragma once

#include "Player.hpp"
#include "communication/Socket.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace server {

class ServerData;

class Client
{
public:
    Client(std::unique_ptr<Socket> socket)
        : _socket(std::move(socket))
    {
        _socket->write("WELCOME\n");
    };

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    Client(Client &&) = default;
    Client &operator=(Client &&) = default;
    ~Client() = default;

    std::size_t writeToClient(const std::string &str);
    bool readFromClient(void);
    std::string getLineFromBuffer(void);

    auto shouldClose(void) const { return _shouldClose; }

    void kill(void) { _shouldClose = true; }

    void close(ServerData &data);

    auto getSocket(void) const { return _socket; }

    auto &getPlayer(void) { return *_player; }

    auto &getPlayer(void) const { return *_player; }

    bool hasPlayer(void) const { return _player != nullptr; }

    void initPlayer(void) { _player = std::make_unique<Player>(); }

    bool isGui(void) const { return _isGui; }

    void setGui(bool isGui) { _isGui = isGui; }

private:
    std::shared_ptr<Socket> _socket;
    bool _shouldClose = false;
    std::string _buffer{};
    std::unique_ptr<Player> _player = nullptr;
    bool _isGui = false;
};

void handlePendingClients(ServerData &data, std::shared_ptr<Client> &client);
void handleActiveClients(ServerData &data, std::shared_ptr<Client> &client);

}
