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

#pragma once

#include <condition_variable>
#include <mutex>

struct Semaphore final {
    Semaphore(unsigned int count = 0)
        : m_count(count)
    {
    }

    inline void notify()
    {
        const std::unique_lock<std::mutex> lock(m_mutex);
        m_count++;
        m_condition_variable.notify_one();
    }

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_count == 0)
            m_condition_variable.wait(lock);

        m_count--;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;
    unsigned int m_count;
};