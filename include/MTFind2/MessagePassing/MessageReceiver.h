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

#include <mutex>

#include "Message.h"

namespace mtfind2 {
/**
 * Components derived from this type implement the push_message() method and
 * any number of overloads with types derived from Message. This makes them
 * receivers of messages, which may allow for inter-component message-passing
 * or for the implementation of components using the producer-consumer paradigm.
 */
struct MessageReceiver {
    /**
     * Pushes a message to this receiver in order to communicate with it.
     * @remarks Implementers are responsible for acquiring/releasing the
     * transaction lock that is unique to each MessageReceiver instance,
     * preferably using std::lock_guard or, for a more generic approach,
     * std::scoped_lock.
     */
    virtual void push_message(const Message &) = 0;

protected:
    std::mutex &transaction_lock() { return m_transaction_lock; }

private:
    std::mutex m_transaction_lock;
};
}