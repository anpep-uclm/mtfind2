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

#include <iostream>
#include <thread>

#include <MTFind2/Search/SearchService.h>
#include <MTFind2/Messages/NoSearchResultsFoundMessage.h>

namespace mtfind2 {
void SearchService::query(Client &client, const SearchRequest &search_request)
{
    /**
     * Any mutation on the content sources vector will take effect in subsequent
     * queries (but not this one).
     */
    std::scoped_lock lock(m_content_sources_lock);
    std::vector<std::thread> threads(m_content_sources.size());

    for (auto *content_source : m_content_sources) {
        threads.emplace_back([this, content_source, &client, &search_request] {
            this->find_in_source(*content_source, client, search_request);
        });
    }

    for (auto &thread : threads) {
        if (thread.joinable())
            thread.join();
    }

    threads.clear();
}

void SearchService::find_in_source(const ContentSource &content_source, Client &client, const SearchRequest &search_request) const
{
}
}