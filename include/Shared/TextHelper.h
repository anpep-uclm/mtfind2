// Copyright (c) 2021 Ángel Pérez <angel@ttm.sh>
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

#include <cctype>
#include <map>
#include <ostream>
#include <algorithm>

struct TextHelper final {
#pragma region Transforms
    static inline void transform_to_lowercase(std::string &str)
    {
        std::string tmp;
        std::transform(str.begin(), str.end(), std::back_inserter(tmp), tolower);
    }
#pragma endregion

#pragma region Search
    using SearchOcurrence = std::tuple<size_t, size_t>;

    static const SearchOcurrence find_in_string(const std::string &haystack, const std::string &needle, size_t start = 0)
    {
        size_t start_pos = haystack.find(needle, start);
        if (start_pos == std::string::npos)
            return std::make_tuple(std::string::npos, std::string::npos);

        return std::make_tuple(start_pos, start_pos + needle.length());
    }

#pragma endregion

#pragma region Contextualization

    static const std::string_view get_surrounding_text(const std::string &source_string, size_t start_pos, size_t end_pos)
    {
        ssize_t prev_word_pos = std::string::npos;
        if (start_pos > 0) {
            prev_word_pos = source_string.rfind(' ', start_pos - 1);
            prev_word_pos = source_string.rfind(' ', prev_word_pos - 1);
        }
        prev_word_pos = std::max(0l, prev_word_pos + 1);

        ssize_t next_word_pos = std::string::npos;
        if (end_pos < source_string.size() - 1)
            next_word_pos = source_string.find(' ', end_pos + 1);

        ssize_t length = std::min(next_word_pos - prev_word_pos, static_cast<ssize_t>(source_string.length()));
        return std::string_view(source_string).substr(prev_word_pos, length);
    }
#pragma endregion
};