/*
** EPITECH PROJECT, 2026
** RlBridge.hpp
** File description:
** Spawn and talk to the Python RL process over stdin/stdout (line based)
*/

#pragma once

#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

namespace ai {

/**
 * @class ProcessException
 * @brief Exception thrown on process-level errors (pipe, fork, exec, write…).
 *
 * Mirrors SocketException: inherits std::runtime_error and preserves the errno
 * value captured at the time of the failure in errorNumber().
 */
class ProcessException : public std::runtime_error
{
public:
    /**
     * @brief Construct a ProcessException.
     * @param message      Human-readable description of the failure.
     * @param errorNumber  errno value at the time of the failure (0 if n/a).
     */
    explicit ProcessException(const std::string &message, int errorNumber = 0) noexcept;

    /**
     * @brief Return the errno value captured at construction time.
     * @return errno value, or 0 if not applicable.
     */
    int errorNumber() const noexcept;

private:
    int _errorNumber;
};

/**
 * @class ProcessClosed
 * @brief Exception thrown when the child process closes its end of a pipe.
 *
 * Distinct from ProcessException so callers can tell a clean child exit
 * (EOF on stdout, EPIPE on stdin) apart from a genuine I/O error — the same
 * way SocketClosed is distinguished from SocketException.
 */
class ProcessClosed : public std::exception
{
public:
    ProcessClosed() noexcept = default;

    /// @return Fixed message "RL process closed the pipe".
    const char *what() const noexcept override { return "RL process closed the pipe"; }
};

/**
 * @class RlBridge
 * @brief Inter-process bridge to a Python reinforcement-learning process.
 *
 * Launches a child process (typically `python3 rl_agent.py`) and exchanges
 * newline-delimited messages with it over two anonymous pipes:
 * - the parent writes one JSON observation per line to the child's **stdin**,
 * - the parent reads one action per line from the child's **stdout**.
 *
 * The child's stderr is left untouched (inherited from the parent) so Python
 * tracebacks remain visible in the terminal for debugging.
 *
 * The class is move-only (copy is deleted) to enforce single ownership of the
 * pipe file descriptors and the child PID. The destructor calls stop(), so a
 * bridge never leaks a file descriptor or a zombie process.
 *
 * ### Typical usage
 * @code
 * RlBridge bridge;
 * bridge.start({"python3", "rl_agent.py"});
 *
 * bridge.send(serialize(observation)); // one JSON observation -> child stdin
 * std::string action = bridge.readLine(); // one action       <- child stdout
 *
 * bridge.stop();
 * @endcode
 *
 * @note start() ignores SIGPIPE process-wide so that writing to a child that
 *       has already exited surfaces as ProcessClosed instead of killing the
 *       whole AI with a signal.
 */
class RlBridge
{
public:
    /// Poll timeout: -1 means block until the fd is ready (no busy-wait).
    /// POLLHUP/POLLERR still fires immediately if the child closes/crashes.
    static constexpr int DEFAULT_TIMEOUT_MS = -1;

    /// @brief Construct an idle bridge (no child running).
    RlBridge() = default;

    /// @brief Stop the child and close every pipe if still running.
    ~RlBridge();

    RlBridge(const RlBridge &) = delete;
    RlBridge &operator=(const RlBridge &) = delete;

    /**
     * @brief Move constructor — transfers ownership of the pipes and PID.
     * @param other  Source bridge; left in an idle state.
     */
    RlBridge(RlBridge &&other) noexcept;

    /**
     * @brief Move assignment — stops the current child, then takes ownership.
     * @param other  Source bridge; left in an idle state.
     */
    RlBridge &operator=(RlBridge &&other) noexcept;

    /**
     * @brief Spawn the Python RL process and wire up the stdin/stdout pipes.
     *
     * Creates two anonymous pipes, fork()s, redirects the child's stdin/stdout
     * onto them with dup2(), then execvp()s @p command in the child. The first
     * element of @p command is the program to run (looked up in `PATH`); the
     * remaining elements are its arguments.
     *
     * @param command  argv of the process to launch, e.g.
     *                 `{"python3", "rl_agent.py", "--model", "ppo"}`.
     *                 Must contain at least the program name.
     * @throws ProcessException if @p command is empty, a bridge is already
     *         running, or pipe()/fork() fails. exec() failures in the child are
     *         reported on the child's stderr and abort the child with code 127.
     */
    void start(const std::vector<std::string> &command);

    /**
     * @brief Send one line to the child's stdin.
     *
     * A trailing `\n` is appended automatically, so @p line must **not** contain
     * one. Loops until every byte is written.
     *
     * @param line       Message to send (without trailing newline).
     * @param timeoutMs  Maximum milliseconds to wait for the pipe to become
     *                   writable on each iteration (default: DEFAULT_TIMEOUT_MS).
     * @throws ProcessException if no child is running or the write fails.
     * @throws ProcessClosed    if the child closed its stdin (EPIPE).
     */
    void send(const std::string &line, int timeoutMs = DEFAULT_TIMEOUT_MS);

    /**
     * @brief Block until a full `\n`-terminated line is read from the child.
     *
     * Uses an internal buffer to reassemble lines split across read() calls.
     * The returned line has its trailing `\n` (and optional `\r`) stripped.
     *
     * @param timeoutMs  Maximum milliseconds to wait for data on each read
     *                   (default: DEFAULT_TIMEOUT_MS).
     * @return The next line from the child's stdout, without line terminator.
     * @throws ProcessException if no child is running or the read fails.
     * @throws ProcessClosed    if the child closed its stdout (EOF).
     */
    std::string readLine(int timeoutMs = DEFAULT_TIMEOUT_MS);

    /**
     * @brief Terminate the child and release every resource.
     *
     * Closes the stdin pipe (signalling EOF to the child), sends SIGTERM, and
     * reaps the child with waitpid() so no zombie is left behind. Closes the
     * stdout pipe and clears the read buffer. Safe to call when no child is
     * running and safe to call multiple times.
     *
     * @return The child's exit status as reported by waitpid(), or -1 if no
     *         child was running.
     */
    int stop();

    /**
     * @brief Return whether a child process is currently running.
     * @return true if start() succeeded and stop() has not been called.
     */
    bool isRunning() const noexcept { return _pid > 0; }

    /**
     * @brief Return the child's process id.
     * @return PID > 0 while running, -1 otherwise.
     */
    pid_t pid() const noexcept { return _pid; }

private:
    /**
     * @brief Wait until @p fd is ready for @p events using poll().
     *
     * @param fd         File descriptor to wait on.
     * @param events     poll event mask (POLLIN or POLLOUT).
     * @param timeoutMs  Maximum wait in milliseconds (-1 to block).
     * @throws ProcessException if poll() itself fails or times out.
     * @throws ProcessClosed    if POLLHUP/POLLERR fires (child gone).
     */
    void pollFd(int fd, short events, int timeoutMs) const;

    /// @brief Close both pipe fds (if open) and reset them to -1.
    void closePipes() noexcept;

    pid_t _pid{-1};      ///< Child PID (-1 when no child is running).
    int _stdinFd{-1};    ///< Write end of the child's stdin pipe.
    int _stdoutFd{-1};   ///< Read end of the child's stdout pipe.
    std::string _buffer; ///< Holds partial reads between readLine() calls.
};

} // namespace ai
