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

#include <MTFind2/MessagePassing/MessageReceiver.h>
#include <MTFind2/Messages/CreditRechargeRequestMessage.h>
#include <MTFind2/Messages/CreditRechargeResponseMessage.h>
#include <Shared/NonCopyable.h>
#include <Shared/NonMoveable.h>

#include <fstream>
#include <string>

namespace mtfind2 {
/**
 * The payment service offers clients a way of recharging credit upon a prior
 * request to do so. It should provide concurrent access to the service but
 * only needs to perform one recharge transaction at a time. For this reason,
 * no job queues are needed and thus, the transaction lock we own as a message
 * receiver suffices. Also, since the payment service perdures throughout the
 * entire lifetime of the application, it is non-copyable and non-moveable.
 */
struct PaymentService final : MessageReceiver, NonCopyable, NonMoveable {
    /**
     * Creates/obtains the global instance of the payment service.
     * @return An instance of this class
     */
    static PaymentService &instance()
    {
        static PaymentService payment_service;
        return payment_service;
    }

    /**
     * Attends a single credit recharge request for a client at a time.
     * @param message Credit recharge request message
     */
    void push_message(const CreditRechargeRequestMessage &message)
    {
        const std::scoped_lock lock(transaction_lock());
        message.client().push_message(CreditRechargeResponseMessage(message.amount()));
    }

    void push_message(const Message &message)
    {
    }

private:
    PaymentService() { }
};
}