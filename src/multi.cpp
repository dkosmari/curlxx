/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/multi.hpp"

#include "curlxx/easy.hpp"


using std::expected;
using std::unexpected;


namespace curl {

    multi::multi()
    {
        create();
    }


    multi::~multi()
        noexcept
    {
        destroy();
    }


    void
    multi::create()
    {
        auto new_raw = curl_multi_init();
        if (!new_raw)
            throw error{"curl_multi_init() failed"};
        destroy();
        acquire(new_raw);
    }


    void
    multi::destroy()
        noexcept
    {
        if (is_valid())
            curl_multi_cleanup(release());
    }


    void
    multi::add(easy& ez)
    {
        auto result = try_add(ez);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    multi::try_add(easy& ez)
        noexcept
    {
        auto e = curl_multi_add_handle(raw, ez.data());
        if (e)
            return unexpected{error{e}};
        return {};
    }


    void
    multi::remove(easy& ez)
    {
        auto result = try_remove(ez);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    multi::try_remove(easy& ez)
        noexcept
    {
        auto e = curl_multi_remove_handle(raw, ez.data());
        if (e)
            return unexpected{error{e}};
        return {};
    }


    unsigned
    multi::perform()
    {
        auto result = try_perform();
        if (!result)
            throw result.error();
        return *result;
    }


    expected<unsigned, error>
    multi::try_perform()
        noexcept
    {
        int running_handles = 0;
        auto e = curl_multi_perform(raw, &running_handles);
        if (e)
            return unexpected{error{e}};
        return running_handles;
    }

} // namespace curl
