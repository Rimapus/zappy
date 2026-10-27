/*
 * EPITECH PROJECT, 2026
 * Buffer.hpp
 * File description:
 * line buffer
 */

#pragma once

#include <string>

/* @brief buffer class */
class Buffer
{
public:
    /**
     * @brief Append data to the buffer.
     *
     * @param data Pointer to the data to append
     * @param len Length of the data to append
     */
    void append(const char *data, std::size_t len);

    /**
     * @brief Pop a line from the buffer if available.
     *
     * @param out String where the popped line will be stored
     * @return true if a line was successfully popped, false elsewise
     */
    bool popLine(std::string &out);

private:
    std::string _buf;
};
