/*
** EPITECH PROJECT, 2026
** System.cpp
** File description:
** Encapsulated system function
*/

#include "System.hpp"
#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

namespace server {

System::SystemException::SystemException(const std::string &operation, int errorNumber)
    : std::runtime_error(operation), _errorNumber(errorNumber)
{
}

int System::SystemException::errorNumber() const noexcept
{
    return _errorNumber;
}

short System::poll(int fd, short events, std::size_t timeout)
{
    if (fd < 0) {
        throw System::SystemException("poll() failed", EBADF);
    }

    struct pollfd config{
        fd,
        events,
        0,
    };

    if (::poll(&config, 1, static_cast<int>(timeout)) == -1 && errno != EINTR) {
        throw System::SystemException("poll() failed", errno);
    }
    return config.revents;
}

void System::poll(std::vector<struct ::pollfd> &list, std::size_t timeout)
{
    if (::poll(list.data(), list.size(), static_cast<int>(timeout)) == -1 && errno != EINTR) {
        throw System::SystemException("poll() failed", errno);
    }
}

std::size_t System::write(int fd, const std::string &str)
{
    std::size_t total = 0;
    const char *data = str.data();
    std::size_t toWrite = str.size();

    while (total < toWrite) {
        auto res = ::write(fd, data + total, toWrite - total);
        if (res == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw System::SystemException("write() failed", errno);
        }
        total += static_cast<std::size_t>(res);
    }
    return total;
}

std::string System::read(int fd, std::size_t size)
{
    std::string buf;
    if (size == 0) {
        return buf;
    }

    buf.resize(size);
    std::size_t total = 0;

    while (total < size) {
        auto res = ::read(fd, buf.data() + total, size - total);
        if (res == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw System::SystemException("read() failed", errno);
        }
        if (res == 0) {
            break;
        }
        total += static_cast<std::size_t>(res);
        break;
    }

    buf.resize(total);
    return buf;
}

}
