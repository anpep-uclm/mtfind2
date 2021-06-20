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

#include <MTFind2/MessagePassing/MessageReceiver.h>
#include <MTFind2/Messages/CreditRechargeRequestMessage.h>
#include <MTFind2/Messages/CreditRechargeResponseMessage.h>
#include <Shared/NonCopyable.h>
#include <Shared/NonMoveable.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <random>
#include <string>
#include <vector>

namespace mtfind2 {
/**
 * The dictionary facility simply picks up a random word collected from the
 * sample corpus included in the `data' directory. Only a single instance is
 * allowed to exist throughout all the program lifetime, thus, being a
 * non-copyable, non-moveable class.
 */
struct Dictionary final : NonCopyable, NonMoveable {
    /**
     * Creates/obtains the global instance of the dictionary.
     * @return An instance of this class
     */
    static Dictionary &instance()
    {
        static Dictionary dictionary;
        return dictionary;
    }

    const std::string &random_word() const
    {
        static std::default_random_engine s_random_engine;
        std::uniform_int_distribution<int> get_random_index(0ul, m_words.size() - 1);
        return m_words[get_random_index(s_random_engine)];
    }

private:
    const std::string k_dictionary_path { "data/dictionary.txt" };

    std::ifstream m_dictionary_stream;
    std::vector<std::string> m_words;

    Dictionary()
    {
        if (!std::filesystem::is_regular_file(k_dictionary_path))
            throw std::runtime_error("The path '" + k_dictionary_path + "' does not point to a regular file.");

        m_dictionary_stream.open(k_dictionary_path, std::ios::in);

        for (std::string word; std::getline(m_dictionary_stream, word);)
            m_words.push_back(std::move(word));
    }
};
}