// mtfind2(1) -- Multi-thread find utility: the sequel that nobody needs
// Copyright (c) 2021 Ángel Pérez <angel.perez7@alu.uclm.es>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <chrono>
#include <Shared/NonCopyable.h>

#include "ContentSource.h"

namespace mtfind2 {
/**
 * Search results are intended to have a short lifetime and thus dont need to
 * be copied around. They represent a single search occurrence that is sent as
 * a message to a client.
 */
struct SearchResult final : NonCopyable {
    SearchResult(
        const ContentSource &content_source,
        size_t line,
        size_t column,
        size_t length)
        : m_content_source(content_source)
        , m_line(line)
        , m_column(column)
        , m_length(length)
        , m_timestamp(std::chrono::steady_clock::now())
    {
    }

    const ContentSource &content_source() const { return m_content_source; }
    size_t line() const { return m_line; }
    size_t column() const { return m_column; }
    size_t length() const { return m_length; }
    const std::chrono::time_point<std::chrono::steady_clock> &timestamp() const { return m_timestamp; }

private:
    const ContentSource &m_content_source;
    const size_t m_line;
    const size_t m_column;
    const size_t m_length;
    const std::chrono::time_point<std::chrono::steady_clock> m_timestamp;
};
}