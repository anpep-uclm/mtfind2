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

#include <Shared/NonCopyable.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <random>

#include "MTFind2/MessagePassing/MessageReceiver.h"
#include "MTFind2/Messages/CreditRechargeResponseMessage.h"
#include "MTFind2/Messages/NoSearchResultsFoundMessage.h"
#include "MTFind2/Messages/NotEnoughCreditMessage.h"
#include "MTFind2/Messages/SearchResultFoundMessage.h"

namespace mtfind2 {
/**
 * Represents a client, this is, a service consumer. It is also capable of
 * receiving Messages so that it can receive search result and subscription
 * credit notifications. Since clients must be unique, it makes little sense
 * for them to be copyable.
 */
struct Client final : MessageReceiver, NonCopyable {
    /**
     * A client with this value as credit is considered as not using credit.
     */
    static constexpr int32_t NotUsingCredit = -1;

    /**
     * Enumerates the different subscription types.
     */
    enum struct SubscriptionType {
        /**
         * Standard (free) clients do not make use of any payment facility.
         * However, they are not prioritized when attending service requests,
         * so their search requests might be deferred until other premium
         * requests have been completed.
         */
        Standard,
        /**
         * Premium clients have a higher priority and might have some form of
         * credit, which they need to recharge in order to keep doing requests.
         */
        Premium
    };

    Client(uint32_t id, SubscriptionType subscription_type, int32_t credit)
        : m_id(id)
        , m_subscription_type(subscription_type)
        , m_credit(credit)
    {
    }

    Client(uint32_t id, SubscriptionType subscription_type)
        : Client(id, subscription_type, NotUsingCredit)
    {
    }

    /**
     * Creates a client with random parameters
     * @return An instance of this class
     */
    static Client create_random()
    {
        static std::atomic<uint32_t> s_last_id(0);

        auto generate_random_boolean = std::bind(std::uniform_int_distribution(0, 1), std::default_random_engine());

        const auto subscription_type = generate_random_boolean() ? SubscriptionType::Premium : SubscriptionType::Standard;
        const auto credit = subscription_type == SubscriptionType::Premium ? 15 : NotUsingCredit;

        return { s_last_id++, subscription_type, credit };
    }

    uint32_t id() const { return m_id; }
    SubscriptionType subscription_type() const { return m_subscription_type; }
    bool has_credit() const { return m_credit > 0; }
    bool has_paid_subscription() const
    {
        return m_subscription_type == SubscriptionType::Premium && m_credit != NotUsingCredit;
    }

    /**
     * Consumes exactly 1 credit.
     */
    void consume_credit()
    {
        if (has_paid_subscription() && has_credit())
            m_credit--;
    }

    void push_message(const NotEnoughCreditMessage &message);
    void push_message(const CreditRechargeResponseMessage &message);
    void push_message(const NoSearchResultsFoundMessage &message);
    void push_message(const SearchResultFoundMessage &message);
    void push_message(const Message &message);

private:
    uint32_t m_id;
    SubscriptionType m_subscription_type;
    int32_t m_credit;
};
}