/*
** EPITECH PROJECT, 2026
** RlBridge.cpp
** File description:
** Spawn and talk to the Python RL process over stdin/stdout (line based)
*/

#include "RlBridge.hpp"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace ai {

// ProcessException

ProcessException::ProcessException(const std::string &message, int errorNumber) noexcept
    : std::runtime_error(message), _errorNumber(errorNumber)
{
}

int ProcessException::errorNumber() const noexcept
{
    return _errorNumber;
}

// RlBridge — lifetime

RlBridge::~RlBridge()
{
    stop();
}

RlBridge::RlBridge(RlBridge &&other) noexcept
    : _pid(other._pid), _stdinFd(other._stdinFd), _stdoutFd(other._stdoutFd), _buffer(std::move(other._buffer))
{
    other._pid = -1;
    other._stdinFd = -1;
    other._stdoutFd = -1;
}

RlBridge &RlBridge::operator=(RlBridge &&other) noexcept
{
    if (this == &other) {
        return *this;
    }
    stop();
    _pid = other._pid;
    _stdinFd = other._stdinFd;
    _stdoutFd = other._stdoutFd;
    _buffer = std::move(other._buffer);
    other._pid = -1;
    other._stdinFd = -1;
    other._stdoutFd = -1;
    return *this;
}

// RlBridge — start / stop

void RlBridge::start(const std::vector<std::string> &command)
{
    if (_pid > 0) {
        throw ProcessException("RL process already running");
    }
    if (command.empty()) {
        throw ProcessException("RL process command is empty");
    }

    std::signal(SIGPIPE, SIG_IGN);

    int inPipe[2] = {-1, -1};
    int outPipe[2] = {-1, -1};
    if (::pipe(inPipe) == -1) {
        throw ProcessException(std::string("pipe() failed: ") + std::strerror(errno), errno);
    }
    if (::pipe(outPipe) == -1) {
        int saved = errno;
        ::close(inPipe[0]);
        ::close(inPipe[1]);
        throw ProcessException(std::string("pipe() failed: ") + std::strerror(saved), saved);
    }

    pid_t pid = ::fork();
    if (pid == -1) {
        int saved = errno;
        ::close(inPipe[0]);
        ::close(inPipe[1]);
        ::close(outPipe[0]);
        ::close(outPipe[1]);
        throw ProcessException(std::string("fork() failed: ") + std::strerror(saved), saved);
    }

    if (pid == 0) {
        ::dup2(inPipe[0], STDIN_FILENO);
        ::dup2(outPipe[1], STDOUT_FILENO);
        ::close(inPipe[0]);
        ::close(inPipe[1]);
        ::close(outPipe[0]);
        ::close(outPipe[1]);

        // Build a NULL-terminated argv for execvp.
        std::vector<char *> argv;
        argv.reserve(command.size() + 1);
        for (const std::string &arg : command) {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(nullptr);

        ::execvp(argv[0], argv.data());
        // Only reached if exec failed.
        std::string err = std::string("execvp(") + command[0] + ") failed: " + std::strerror(errno) + "\n";
        ssize_t ignored = ::write(STDERR_FILENO, err.c_str(), err.size());
        (void)ignored;
        _exit(127);
    }

    ::close(inPipe[0]);
    ::close(outPipe[1]);
    _stdinFd = inPipe[1];
    _stdoutFd = outPipe[0];
    _pid = pid;
    _buffer.clear();
}

int RlBridge::stop()
{
    if (_pid <= 0) {
        closePipes();
        return -1;
    }

    if (_stdinFd >= 0) {
        ::close(_stdinFd);
        _stdinFd = -1;
    }

    ::kill(_pid, SIGTERM);

    int status = 0;
    pid_t reaped;
    do {
        reaped = ::waitpid(_pid, &status, 0);
    } while (reaped == -1 && errno == EINTR);

    _pid = -1;
    closePipes();
    return status;
}

// RlBridge — I/O

void RlBridge::send(const std::string &line, int timeoutMs)
{
    if (_pid <= 0 || _stdinFd < 0) {
        throw ProcessException("RL process is not running", EBADF);
    }

    std::string payload = line + "\n";
    const char *ptr = payload.data();
    std::size_t remaining = payload.size();

    while (remaining > 0) {
        pollFd(_stdinFd, POLLOUT, timeoutMs);

        ssize_t written = ::write(_stdinFd, ptr, remaining);
        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EPIPE) {
                throw ProcessClosed();
            }
            throw ProcessException(std::string("write() failed: ") + std::strerror(errno), errno);
        }
        ptr += written;
        remaining -= static_cast<std::size_t>(written);
    }
}

std::string RlBridge::readLine(int timeoutMs)
{
    if (_pid <= 0 || _stdoutFd < 0) {
        throw ProcessException("RL process is not running", EBADF);
    }

    while (true) {
        auto pos = _buffer.find('\n');
        if (pos != std::string::npos) {
            std::string line = _buffer.substr(0, pos);
            _buffer.erase(0, pos + 1);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            return line;
        }

        pollFd(_stdoutFd, POLLIN, timeoutMs);

        char chunk[4096];
        ssize_t n = ::read(_stdoutFd, chunk, sizeof(chunk));
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw ProcessException(std::string("read() failed: ") + std::strerror(errno), errno);
        }
        if (n == 0) {
            if (!_buffer.empty()) {
                std::string line = std::move(_buffer);
                _buffer.clear();
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                return line;
            }
            throw ProcessClosed();
        }
        _buffer.append(chunk, static_cast<std::size_t>(n));
    }
}

// RlBridge — internal

void RlBridge::pollFd(int fd, short events, int timeoutMs) const
{
    struct pollfd pfd{};
    pfd.fd = fd;
    pfd.events = events;

    int ret = ::poll(&pfd, 1, timeoutMs);
    if (ret == -1) {
        if (errno == EINTR) {
            return;
        }
        throw ProcessException(std::string("poll() failed: ") + std::strerror(errno), errno);
    }
    if (ret == 0) {
        throw ProcessException("RL process timed out", ETIMEDOUT);
    }
    if ((pfd.revents & (POLLHUP | POLLERR)) && !(pfd.revents & POLLIN)) {
        throw ProcessClosed();
    }
}

void RlBridge::closePipes() noexcept
{
    if (_stdinFd >= 0) {
        ::close(_stdinFd);
        _stdinFd = -1;
    }
    if (_stdoutFd >= 0) {
        ::close(_stdoutFd);
        _stdoutFd = -1;
    }
}

} // namespace ai
