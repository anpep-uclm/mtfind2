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

#include <MTFind2/Client/Client.h>
#include <MTFind2/Messages/NotEnoughCreditMessage.h>
#include <MTFind2/Messages/SearchResultFoundMessage.h>
#include <MTFind2/Search/SearchService.h>
#include <Shared/TextHelper.h>
#include <Shared/Semaphore.h>

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
    size_t line_number = 1;
    std::string lowercase_query { search_request.query() };
    TextHelper::transform_to_lowercase(lowercase_query);

    for (const std::string &line : content_source.lines()) {
        std::string lowercase_line { line };
        TextHelper::transform_to_lowercase(lowercase_line);

        for (auto [start_pos, end_pos] = std::make_tuple(0, 0);
             start_pos != std::string::npos;) {
            if (!client.has_credit()) {
                Semaphore semaphore;
                client.push_message(NotEnoughCreditMessage(semaphore));
                if (client.subscription_type() == Client::SubscriptionType::Standard)
                    return;

                // Wait for credit recharge if user is premium
                semaphore.wait();
                std::cout << search_request << ": resuming search request after credit recharge" << std::endl;
            }

            std::tie(start_pos, end_pos) = TextHelper::find_in_string(lowercase_line, lowercase_query, end_pos);
            if (start_pos == std::string::npos)
                break;

            bool is_final_result = false;
            if (line_number == content_source.lines().size()) {
                auto [next_start_pos, _] = TextHelper::find_in_string(lowercase_line, lowercase_query, end_pos);
                is_final_result = next_start_pos == std::string::npos;
            }

            client.consume_credit();
            client.push_message(SearchResultFoundMessage(search_request, SearchResult(content_source, TextHelper::get_surrounding_text(line, start_pos, end_pos), line_number, start_pos + 1, search_request.query().length(), is_final_result)));
        }

        line_number++;
    }
}
}