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

#include <MTFind2/MessagePassing/Message.h>
#include <Shared/Semaphore.h>

namespace mtfind2 {

/**
 * Message passed by a client to the payment system to requests the recharge of
 * a specific amount of credits.
 */
struct CreditRechargeRequestMessage final : private Message {
    CreditRechargeRequestMessage(Client &client, Semaphore &semaphore, size_t amount)
        : m_client(client)
        , m_amount(amount)
        , m_semaphore(semaphore)
    {
    }

    Client &client() const { return m_client; }
    size_t amount() const { return m_amount; }
    Semaphore &semaphore() const { return m_semaphore; }

private:
    Client &m_client;
    const size_t m_amount;
    Semaphore &m_semaphore;
};
}