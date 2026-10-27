/*
** EPITECH PROJECT, 2026
** Loop.cpp
** File description:
** Main loop function
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Gui.hpp"
#include "Logger.hpp"
#include "Main.hpp"
#include "PeriodicEvents.hpp"
#include "communication/Socket.hpp"
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <format>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <sys/poll.h>
#include <sys/socket.h>
#include <utility>
#include <vector>

static volatile std::sig_atomic_t INTERRUPTED = false;

static void interruptHandler(int)
{
    INTERRUPTED = true;
}

namespace server {

static std::unique_ptr<Socket> createServerSocket(std::uint16_t port)
{
    auto socket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    socket->setReuseAddress();
    socket->bind(*reinterpret_cast<const sockaddr *>(&addr));
    socket->listen();

    return socket;
}

auto initData(const Config &config)
{
    ServerData data{
        createServerSocket(config._port),
        config._teamNames,
        config._frequency};
    data.setMap(config._width, config._height);
    data.getMap().spawnNaturalResources(data);
    data.getMap().spawnEggs(data.getTeams(), config._clientsNumber);

    for (const auto &egg : data.getMap().getEggs()) {
        data.getGui().broadcast(std::format("enw #{} #-1 {} {}\n",
                                            egg.getId(), egg.getX(), egg.getY()));
    }

    return data;
}

auto mainPoll(ServerData &data)
{
    auto timeout = -1;
    auto &gameEvents = data.getGameEvents();
    auto &clients = data.getClientsData();
    std::vector<std::shared_ptr<Socket>> listSocket;

    listSocket.push_back(data.getServerSocket());
    for (const auto &client : clients._pendingClients) {
        listSocket.push_back(client->getSocket());
    }
    for (const auto &client : clients._activeClients) {
        listSocket.push_back(client->getSocket());
    }
    for (const auto &client : data.getGui().getClients()) {
        listSocket.push_back(client->getSocket());
    }

    if (!gameEvents.empty()) {
        auto now = std::chrono::steady_clock::now();
        auto nextEventTime = gameEvents.top()._executionTime;

        if (nextEventTime <= now) {
            timeout = 0;
        } else {
            auto duration = nextEventTime - now;
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            timeout = static_cast<int>(durationMs.count());
        }
    }

    return Socket::pollMultiple(listSocket, POLLIN, timeout);
}

void handleGameEvents(ServerData &data)
{
    auto &gameEvents = data.getGameEvents();
    auto now = std::chrono::steady_clock::now();

    while (!gameEvents.empty() && gameEvents.top()._executionTime <= now) {
        auto event = gameEvents.top();

        gameEvents.pop();
        auto owner = event.getOwner();

        if (owner && owner->getPlayer().isFrozen()) {
            owner->getPlayer().pauseEvent(std::move(event));
            now = std::chrono::steady_clock::now();
            continue;
        }
        event._callback();
        now = std::chrono::steady_clock::now();
    }
}

void handleServerSocket(ServerData &data, std::optional<short> res)
{
    if (!res.has_value() || !(*res & POLLIN)) {
        return;
    }
    auto newSocket = data.getServerSocket()->accept();
    auto newClient = std::make_unique<Client>(std::move(newSocket.first));
    Logger::info("New client connected.");
    data.getClientsData()._pendingClients.push_back(std::move(newClient));
};

void handlePollRevent(ServerData &data, std::vector<std::optional<short>> &pollRes)
{
    auto &gui = data.getGui().getClients();
    auto &pending = data.getClientsData()._pendingClients;
    auto &active = data.getClientsData()._activeClients;

    for (long long i = static_cast<long long>(gui.size()) - 1; i >= 0; i--) {
        if (pollRes.back().has_value()) {
            handleGuiClients(data, gui.at(i));
        }
        pollRes.pop_back();

        if (gui.at(i)->shouldClose()) {
            Logger::info("GUI client disconnected.");
            gui.erase(gui.begin() + i);
            continue;
        }
    }

    for (long long i = static_cast<long long>(active.size()) - 1; i >= 0; i--) {
        if (pollRes.back().has_value()) {
            handleActiveClients(data, active.at(i));
        }

        pollRes.pop_back();

        if (active.at(i)->shouldClose()) {
            active.at(i)->close(data);
            active.erase(active.begin() + i);
            continue;
        }
    }

    for (long long i = static_cast<long long>(pending.size()) - 1; i >= 0; i--) {
        if (pollRes.back().has_value()) {
            handlePendingClients(data, pending.at(i));
        }
        pollRes.pop_back();

        if (pending.at(i)->shouldClose()) {
            Logger::info("Pending client disconnected.");
            pending.at(i)->close(data);
            pending.erase(pending.begin() + i);
            continue;
        }
        if (pending.at(i)->isGui()) {
            gui.push_back(std::move(pending.at(i)));
            pending.erase(pending.begin() + i);
            continue;
        }
        if (pending.at(i)->hasPlayer()) {
            active.push_back(std::move(pending.at(i)));
            pending.erase(pending.begin() + i);
            continue;
        }
    }

    if (pollRes.back().has_value()) {
        handleServerSocket(data, pollRes.back());
    }
}

int run(const Config &config)
{
    try {
        ServerData data = initData(config);

        Logger::info("Server started on port {}.", config._port);
        std::signal(SIGINT, interruptHandler);
        while (!data.getFinished() && !INTERRUPTED) {
            processPeriodicEvents(data);
            auto pollRes = mainPoll(data);
            handlePollRevent(data, pollRes);
            handleGameEvents(data);
        }
        Logger::info("Server shutting down.");
        data.getGui().broadcast("smg Server is closing\n");

    } catch (const std::exception &e) {
        Logger::error("Fatal error: {}.", e.what());
        return 84;
    }
    return 0;
}

}
