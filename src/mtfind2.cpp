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
#include <Shared/TextHelper.h>

using namespace mtfind2;
using namespace std::chrono_literals;

#pragma region Signal handling

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

#pragma endregion

static void add_sample_content_sources(std::vector<SearchService> &search_services)
{
    for (const auto &entry : std::filesystem::directory_iterator("data")) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            const auto *content_source = new ContentSource(entry.path().string());
            for (auto &search_service : search_services)
                search_service.add_content_source(*content_source);
        }
    }
}

int main(int argc, char *argv[])
{
#ifdef __unix__
    signal(SIGINT, signal_handler);
#endif

    // Initialize search services
    const auto num_cores = std::thread::hardware_concurrency();
    std::vector<SearchService> search_services(num_cores);
    add_sample_content_sources(search_services);

    // Create search proxy for concurrent and parallel search resolution
    SearchProxy search_proxy;
    for (auto &search_service : search_services)
        search_proxy.add_search_service(search_service);

    // Create thread for mocking search requests continuously
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

    search_proxy.start();
    mock_thread.join();
    search_proxy.stop();

    return 0;
}