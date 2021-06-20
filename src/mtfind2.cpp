// mtfind2(1) -- Multi-thread find utility: the sequel that nobody needs
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

#include <chrono>
#include <iostream>
#include <thread>
#ifdef __unix__
#include <csignal>
#endif

#include <MTFind2/Search/SearchProxy.h>
#include <MTFind2/Search/SearchService.h>

using namespace mtfind2;
using namespace std::chrono_literals;

static std::atomic<bool> g_keep_running = true;

#ifdef __unix__
static void signal_handler(int signal_num)
{
    if (signal_num == SIGINT) {
        g_keep_running = false;
        std::cout << "received SIGINT" << std::endl;
    }
}
#endif // __unix__

int main(int argc, char *argv[])
{
#ifdef __unix__
    signal(SIGINT, signal_handler);
#endif

    const auto num_cores = std::thread::hardware_concurrency();
    std::vector<SearchService> search_services(num_cores);

    const ContentSource content_sources[] = {
        { "data/La-última-sirena.txt" },
        { "data/prueba.txt" },
        { "data/VIVE-TU-SUEÑO.txt" }
    };

    SearchProxy search_proxy;
    for (size_t i = 0; i < num_cores; i++) {
        for (const auto &content_source : content_sources)
            search_services[i].add_content_source(content_source);
        search_proxy.add_search_service(search_services[i]);
    }


    std::thread mock_thread([&search_proxy]() {
        const size_t search_request_count = 15;
        const auto period = 2s;

        while (g_keep_running) {
            for (size_t i = 0; i < search_request_count; i++) {
                auto client = Client::create_random();
                auto search_request = SearchRequest::create_random();
                search_proxy.query(*client, *search_request);
            }

            std::this_thread::sleep_for(period);
        }
    });

    std::thread search_thread([&search_proxy]() {
        while (g_keep_running)
            search_proxy.serve();
    });

    search_thread.join();
    mock_thread.join();

    return 0;
}