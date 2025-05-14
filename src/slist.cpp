/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <new>

#include "curlxx/slist.hpp"


namespace curl {

    slist::~slist()
        noexcept
    {
        destroy();
    }


    void
    slist::destroy()
        noexcept
    {
        if (is_valid())
            curl_slist_free_all(release());
    }


    void
    slist::append(const std::string& str)
    {
        auto new_raw = curl_slist_append(raw, str.data());
        if (!new_raw)
            throw std::bad_alloc{};
        acquire(new_raw);
    }

} // namespace curl
