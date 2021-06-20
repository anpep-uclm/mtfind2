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

#include <memory>
#include <vector>

#include "SearchProvider.h"

namespace mtfind2 {
/**
 * Basic search service implementation. Its responsibility is that of attending
 * search queries directly by allocating all required resources and using them
 * all at once to provide a search result or notify any kind of errors.
 * If you are looking for a search provider that prioritizes clients based on
 * subscription types, credits and enqueues search requests to more efficiently
 * manage compute and memory resources, see the SearchProxy class.
 */
struct SearchService final : private SearchProvider {
    /**
     * Performs a search query on all registered content sources.
     * @param client Client that issued this search request
     * @param search_request Search request object
     */
    void query(const Client &client, const SearchRequest &search_request);

    void add_content_source(const ContentSource &content_source)
    {
        const std::scoped_lock lock(m_content_sources_lock);
        m_content_sources.push_back(&content_source);
    }

private:
    /**
     * Performs a single-thread query on a specific content source.
     * @param content_source Content source where the search term will be looked up
     * @param client Client that issued this search request
     * @param search_request Search request object
     */
    void query_single_thread(const SearchService &content_source, const Client &client, const SearchRequest &search_request) const;

    /**
     * Content sources are not bound to a SearchService instance. This is by-design,
     * because more than one instance of SearchService may be created (e.g. for
     * parallelism) and shared ownership of these objects is unnecessarily complex.
     */
    std::vector<const ContentSource *> m_content_sources;

    /**
     * Not really needed (when using SearchProxy), but we are cautious enough
     * to lock content sources when modifying or iterating over.
     */
    std::mutex m_content_sources_lock;
};
}