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

#include "MTFind2/MessagePassing/Message.h"
#include "MTFind2/Search/SearchRequest.h"
#include "MTFind2/Search/SearchResult.h"

namespace mtfind2 {
/**
 * Message passed from a search provider to a client that performed a search
 * request to notify the finding of a single occurrence.
 */
struct SearchResultFoundMessage final : private Message {
    SearchResultFoundMessage(const SearchRequest &search_request, const SearchResult &search_result)
        : m_search_request(search_request)
        , m_search_result(search_result)
    {
    }

    const SearchRequest &search_request() const { return m_search_request; }
    const SearchResult &search_result() const { return m_search_result; }

private:
    const SearchRequest &m_search_request;
    const SearchResult &m_search_result;
};
}