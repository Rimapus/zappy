/*
** EPITECH PROJECT, 2026
** test_helpers.hpp
** File description:
** Test helpers for unit tests
*/

#pragma once

#include "Client.hpp"
#include "Data.hpp"
#include "GameEvent.hpp"
#include "communication/Socket.hpp"
#include <cstddef>
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace server::test {

inline std::unique_ptr<Socket> createDummySocket()
{
    return std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
}

inline ServerData createTestServerData(
    std::size_t freq = 100000,
    std::size_t width = 10,
    std::size_t height = 10,
    std::vector<std::string> teams = {"team1"})
{
    ServerData data(createDummySocket(), std::move(teams), freq);
    data.setMap(width, height);
    return data;
}

inline std::pair<std::shared_ptr<Client>, int> createTestClient()
{
    int cl[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, cl) != 0) {
        throw std::runtime_error("socketpair failed for test client");
    }
    auto clientSock = std::make_unique<Socket>(cl[0]);
    auto client = std::make_shared<Client>(std::move(clientSock));
    client->initPlayer();
    int testFd = cl[1];

    char welcome[16];
    auto n = read(testFd, welcome, sizeof(welcome));
    (void)n;

    return {std::move(client), testFd};
}

inline void addToActiveClients(ServerData &data, std::shared_ptr<Client> &client)
{
    data.getClientsData()._activeClients.push_back(client);
}

inline std::string readAll(int fd)
{
    std::string result;
    char buf[4096];
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    while (true) {
        auto n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0) {
            break;
        }
        buf[n] = '\0';
        result.append(buf, n);
    }
    fcntl(fd, F_SETFL, flags);
    return result;
}

inline void executeGameEvent(ServerData &data)
{
    auto &events = data.getGameEvents();
    if (events.empty()) {
        return;
    }
    GameEvent event = events.top();
    events.pop();
    if (event._callback) {
        event._callback();
    }
}

inline void closeTestFd(int fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

}
