/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <new>

#include "curlxx/slist.hpp"

#include "utils.hpp"


using curl::utils::value_or_throw;


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
        return value_or_throw(try_append(str));
    }


    std::expected<void, error>
    slist::try_append(const std::string& str)
        noexcept
    {
        auto new_raw = curl_slist_append(raw, str.data());
        if (!new_raw)
            return std::unexpected{error{CURLE_OUT_OF_MEMORY}};
        acquire(new_raw);
        return {};
    }

} // namespace curl
