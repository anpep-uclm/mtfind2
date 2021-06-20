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

#include <MTFind2/Client/Client.h>
#include <MTFind2/Messages/CreditRechargeResponseMessage.h>
#include <MTFind2/Messages/NoSearchResultsFoundMessage.h>
#include <MTFind2/Messages/NotEnoughCreditMessage.h>
#include <MTFind2/Messages/SearchResultFoundMessage.h>
#include <MTFind2/Payment/PaymentService.h>
#include <iostream>

namespace mtfind2 {
void Client::push_message(const NotEnoughCreditMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    std::cerr << "no enough credit" << std::endl;
    PaymentService::instance().push_message(CreditRechargeRequestMessage(*this, 15));
}

void Client::push_message(const CreditRechargeResponseMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    std::cout << "got " << message.amount() << " in credit" << std::endl;
    m_credit += message.amount();
}

void Client::push_message(const NoSearchResultsFoundMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    std::cerr << "no results from " << message.search_request().query();
}

void Client::push_message(const SearchResultFoundMessage &message)
{
    const std::scoped_lock lock(transaction_lock());
    std::cout << "search result for " << message.search_request().query() << std::endl;
}

void Client::push_message(const Message &message)
{
    const std::scoped_lock lock(transaction_lock());
}
}