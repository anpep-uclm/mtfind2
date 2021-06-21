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

#include <chrono>
#include <iostream>

#include <MTFind2/Client/Client.h>
#include <MTFind2/Messages/CreditRechargeResponseMessage.h>
#include <MTFind2/Messages/NoSearchResultsFoundMessage.h>
#include <MTFind2/Messages/NotEnoughCreditMessage.h>
#include <MTFind2/Messages/SearchResultFoundMessage.h>
#include <MTFind2/Payment/PaymentService.h>
#include <Shared/TextHelper.h>

namespace mtfind2 {
void Client::push_message(const NotEnoughCreditMessage &message)
{
    // Don't lock transaction! Doing so will prevent CreditRechargeResponseMessage
    // to get through and complete the operation, resulting in a deadlock!
    std::cout << tag() << ": requesting more credit" << std::endl;
    PaymentService::instance().push_message(CreditRechargeRequestMessage(*this, message.semaphore(), 15));
}

void Client::push_message(const CreditRechargeResponseMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    if (message.amount() == 0) {
        std::cerr << tag() << ": ran out of credit!" << std::endl;
    } else {
        std::cout << tag() << ": got " << message.amount() << " in credit" << std::endl;
        m_credit += message.amount();
    }
}

void Client::push_message(const NoSearchResultsFoundMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    std::cerr << tag() << ": no results from " << message.search_request().query() << std::endl;
}

void Client::push_message(const SearchResultFoundMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    const auto &search_result = message.search_result();
    const auto &search_request = message.search_request();
    std::cout << search_request << ": " << search_result.content_source() << ": line " << search_result.line() << ", column " << search_result.column() << ": ..." << search_result.surrounding_text() << "...";
    if (search_result.is_final_result())
        std::cout << " (search yielded no more results)";
    std::cout << std::endl;

    const auto response_time = search_result.timestamp() - search_request.timestamp();
    std::cout << "total response time: " << std::chrono::duration<double, std::milli>(response_time).count() << "ms" << std::endl;
}

void Client::push_message(const Message &message)
{
    const std::scoped_lock lock(transaction_lock());
}
}