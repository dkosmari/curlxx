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

    slist::slist(std::initializer_list<value_type> values)
    {
        for (auto entry : values)
            append(entry);
    }


    slist::slist(std::initializer_list<std::string> values)
    {
        for (auto entry : values)
            append(entry);
    }


    slist::slist(const slist& other)
    {
        for (auto& entry : other)
            append(entry);
    }


    slist&
    slist::operator =(const slist& other)
    {
        if (this != &other)
            for (auto& entry : other)
                append(entry);
        return *this;
    }


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
    slist::append(value_type str)
    {
        return value_or_throw(try_append(str));
    }


    void
    slist::append(const std::string& str)
    {
        return value_or_throw(try_append(str));
    }


    std::expected<void, error>
    slist::try_append(value_type str)
        noexcept
    {
        auto new_raw = curl_slist_append(raw, str);
        if (!new_raw)
            return std::unexpected{error{CURLE_OUT_OF_MEMORY}};
        acquire(new_raw);
        return {};
    }


    std::expected<void, error>
    slist::try_append(const std::string& str)
        noexcept
    {
        return try_append(str.data());
    }


    slist::const_iterator
    slist::begin()
        const noexcept
    {
        return const_iterator{raw};
    }


    slist::const_iterator
    slist::cbegin()
        const noexcept
    {
        return begin();
    }


    slist::const_iterator
    slist::end()
        const noexcept
    {
        return {};
    }


    slist::const_iterator
    slist::cend()
        const noexcept
    {
        return end();
    }


    slist::size_type
    slist::size()
        const noexcept
    {
        return std::distance(begin(), end());
    }

} // namespace curl
