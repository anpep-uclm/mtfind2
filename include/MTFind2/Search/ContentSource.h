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

#include <Shared/NonCopyable.h>
#include <Shared/Tagged.h>

#include <fstream>
#include <iostream>
#include <string>

namespace mtfind2 {
/**
 * Content sources map to plain text files where search terms will be looked
 * for in. These are initialized on startup and last until program termination.
 * Since its lifetime is permanent it makes no sense for them to be copied around.
 */
struct ContentSource final : NonCopyable, Tagged<std::string> {
    ContentSource(std::string file_path)
        : m_file_path(std::move(file_path))
        , m_stream(m_file_path, std::ios::in) // Open read-only
    {
        for (std::string line; std::getline(m_stream, line);)
            m_lines.push_back(std::move(line));
        std::cout << tag() << ": " << m_lines.size() << " line(s) read" << std::endl;
        m_stream.close();
    }

    const std::string tag() const { return "ContentSource(\"" + m_file_path + "\")"; }
    const std::vector<std::string> &lines() const { return m_lines; }

private:
    const std::string m_file_path;
    std::ifstream m_stream;
    std::vector<std::string> m_lines;
};
}