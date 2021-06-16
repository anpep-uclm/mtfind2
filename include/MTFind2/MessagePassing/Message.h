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

namespace mtfind2 {
/**
 * Messages are meant to provide a form of communication between the different
 * components of the program. Their lifetime should be fairly limited since
 * these objects only need to exist for service requests or responses. Making
 * these non-copyable ensures they are used as we are intending to.
 *
 * Also, when creating new message types, make sure their members stay immutable
 * and avoid messages from having complex types. Also, try to use references
 * instead of object instances whenever possible.
 */
struct Message : private NonCopyable {
};
}