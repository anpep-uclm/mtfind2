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

#include <map>
#include <queue>
#include <tuple>

#include "MTFind2/Client/Client.h"
#include "SearchProvider.h"
#include "SearchRequest.h"
#include "SearchService.h"

namespace mtfind2 {
using SearchTask = std::tuple<Client &, const SearchRequest &>;

struct SearchTaskCompare final {
    bool operator()(const SearchTask &lhs, const SearchTask &rhs)
    {
        return std::get<1>(lhs).timestamp() < std::get<1>(rhs).timestamp();
    }
};

/**
 * Basic search service implementation. Its responsibility is that of attending
 * search queries directly by allocating all required resources and using them
 * all at once to provide a search result or notify any kind of errors.
 * If you are looking for a search provider that prioritizes clients based on
 * subscription types, credits and enqueues search requests to more efficiently
 * manage compute and memory resources, see the SearchProxy class.
 */
struct SearchProxy final : private SearchProvider {
    void query(const Client &client, const SearchRequest &search_request)
    {
    }

    void add_search_service(const SearchService &search_service)
    {
        const std::scoped_lock lock(m_search_services_lock);
        m_search_services.push_back(&search_service);
    }

private:
    std::vector<const SearchService *> m_search_services;
    std::map<Client::SubscriptionType, std::priority_queue<SearchTask, std::vector<SearchTask>, SearchTaskCompare>> m_job_queues;

    /**
     * Not really needed (when using SearchProxy), but we are cautious enough
     * to lock content sources when modifying or iterating over.
     */
    std::mutex m_search_services_lock;
};
}