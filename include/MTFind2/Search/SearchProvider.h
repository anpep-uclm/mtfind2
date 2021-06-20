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
#include <Shared/NonMoveable.h>

#include "MTFind2/Client/Client.h"
#include "SearchRequest.h"

namespace mtfind2 {
/**
 * Generic search provider interface. Search providers are intended to be
 * single-instance and their lifetime span to the total runtime of the program.
 * This makes them good candidates for being single-instance objects that
 * should not be copied or moved around.
 */
struct SearchProvider : NonCopyable, NonMoveable {
    virtual void query(const Client &client, const SearchRequest &search_request) = 0;
};
}