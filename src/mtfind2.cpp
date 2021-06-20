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
#include <future>
#include <iostream>
#include <thread>

#include <MTFind2/Client/Client.h>
#include <MTFind2/Search/SearchProxy.h>
#include <MTFind2/Search/SearchService.h>

using namespace mtfind2;
using namespace std::chrono_literals;

static void mock_search_requests(
    const SearchProvider &search_provider,
    int client_count = 1,
    int search_request_count = 1,
    const std::chrono::duration<int64_t> period = 2s)
{

}

int main(int argc, char *argv[])
{
    const int num_cores = std::thread::hardware_concurrency();
    const auto search_services = new SearchService[num_cores];
    const ContentSource content_sources[] = {
        { "data/La-última-sirena.txt" },
        { "data/prueba.txt" },
        { "data/VIVE-TU-SUEÑO.txt" }
    };

    SearchProxy search_proxy;
    for (int i = 0; i < num_cores; i++) {
        for (const auto &content_source : content_sources)
            search_services[i].add_content_source(content_source);
        search_proxy.add_search_service(search_services[i]);
    }

    Client client(42, mtfind2::Client::SubscriptionType::Standard);
    client.push_message(mtfind2::NotEnoughCreditMessage());
    client.push_message(mtfind2::CreditRechargeResponseMessage(42));

    return 0;
}