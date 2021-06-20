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

#include "../Client/Client.h"
#include "SearchProvider.h"
#include "SearchRequest.h"
#include "SearchService.h"

namespace mtfind2 {
using SearchTask = std::pair<Client *, const SearchRequest *>;

struct SearchTaskCompare final {
    bool operator()(const SearchTask &lhs, const SearchTask &rhs)
    {
        return std::get<1>(lhs)->timestamp() < std::get<1>(rhs)->timestamp();
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
    explicit SearchProxy()
        : m_keep_running()
        , m_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    void query(Client &client, const SearchRequest &search_request)
    {
        const std::scoped_lock lock(m_queue_locks[client.subscription_type()]);
        m_queues[client.subscription_type()].push(SearchTask(&client, &search_request));
    }

    void add_search_service(SearchService &search_service)
    {
        if (m_keep_running)
            throw std::runtime_error("Can't add search service while search proxy is running");

        const std::scoped_lock lock(m_search_services_lock);
        m_search_services.push_back(&search_service);
    }

    void start()
    {
        const std::scoped_lock lock(m_search_services_lock);
        m_keep_running = true;
        m_thread_pool.clear();

        for (auto *search_service : m_search_services) {
            m_thread_pool.emplace_back([this, search_service] {
                while (m_keep_running)
                    this->handle_service_request(*search_service);
            });
        }
    }

    void stop()
    {
        const std::scoped_lock lock(m_search_services_lock);
        m_keep_running = false;

        for (auto &thread : m_thread_pool) {
            if (thread.joinable())
                thread.join();
        }

        m_thread_pool.clear();
    }

private:
    /**
     * We declare random engine instance and uniform random generator as members
     * because declaring them as static variables on a concurrent method is not
     * thread-safe and would require extra synchronization.
     */
    std::default_random_engine m_random_engine;
    std::uniform_real_distribution<float> generate_random_float;

    bool m_keep_running;
    std::vector<SearchService *> m_search_services;
    std::map<Client::SubscriptionType, std::priority_queue<SearchTask, std::vector<SearchTask>, SearchTaskCompare>> m_queues;
    std::map<Client::SubscriptionType, std::mutex> m_queue_locks;
    std::vector<std::thread> m_thread_pool;

    /**
     * Not really needed (when using SearchProxy), but we are cautious enough
     * to lock content sources when modifying or iterating over.
     */
    std::mutex m_search_services_lock;

    void handle_service_request(SearchService &search_service)
    {
        Client::SubscriptionType key;
        float p = generate_random_float(m_random_engine);
        if (p <= 0.8)
            key = Client::SubscriptionType::Premium;
        else
            key = Client::SubscriptionType::Standard;

        Client *client_ptr;
        SearchRequest const *search_request_ptr;
        {
            const std::scoped_lock lock(m_queue_locks[key]);
            auto &queue = m_queues[key];

            if (queue.empty())
                return;

            std::tie(client_ptr, search_request_ptr) = queue.top();
            std::cout << "[" << std::this_thread::get_id() << "] " << *search_request_ptr << std::endl;
            queue.pop();
        }

        search_service.query(*client_ptr, *search_request_ptr);
    }
};
}