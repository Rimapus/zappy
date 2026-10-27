/*
 * EPITECH PROJECT, 2026
 * Buffer.cpp
 * File description:
 * line buffer
 */

#include "Buffer.hpp"

/**
 * @brief Append raw bytes to the internal buffer.
 *
 * @param data Pointer to the data to append
 * @param len  Number of bytes to append
 */
void Buffer::append(const char *data, std::size_t len)
{
    _buf.append(data, len);
}

/**
 * @brief Extract the next complete newline-delimited line from the buffer.
 *
 * @param out Receives the line content (without the newline or trailing \r)
 * @return true if a line was extracted, false if no complete line is available yet
 */
bool Buffer::popLine(std::string &out)
{
    std::size_t pos = _buf.find('\n');

    if (pos == std::string::npos) {
        return false;
    }
    out = _buf.substr(0, pos);
    _buf.erase(0, pos + 1);
    if (!out.empty() && out.back() == '\r') {
        out.pop_back();
    }
    return true;
}
