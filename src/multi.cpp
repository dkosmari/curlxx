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


    std::vector<multi::msg_done>
    multi::get_done()
    {
        std::vector<msg_done> result;
        int pending;
        while (auto msg = curl_multi_info_read(raw, &pending)) {
            // ignore unknown messages
            if (msg->msg != CURLMSG_DONE)
                continue;
            result.emplace_back(easy::get_wrapper(msg->easy_handle),
                                msg->data.result);
        }
        return result;
    }


    void
    multi::set_max_connections(long n)
    {
        auto result = try_set_max_connections(n);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    multi::try_set_max_connections(long n)
        noexcept
    {
        auto e = curl_multi_setopt(raw, CURLMOPT_MAXCONNECTS, n);
        if (e)
            return unexpected{error{e}};
        return {};
    }


    void
    multi::set_max_concurrent_streams(long n)
    {
        auto result = try_set_max_concurrent_streams(n);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    multi::try_set_max_concurrent_streams(long n)
        noexcept
    {
        auto e = curl_multi_setopt(raw, CURLMOPT_MAX_CONCURRENT_STREAMS, n);
        if (e)
            return unexpected{error{e}};
        return {};
    }


    void
    multi::set_max_host_connections(long n)
    {
        auto result = try_set_max_host_connections(n);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    multi::try_set_max_host_connections(long n)
        noexcept
    {
        auto e = curl_multi_setopt(raw, CURLMOPT_MAX_HOST_CONNECTIONS, n);
        if (e)
            return unexpected{error{e}};
        return {};
    }


    void
    multi::set_max_total_connections(long n)
    {
        auto result = try_set_max_total_connections(n);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    multi::try_set_max_total_connections(long n)
        noexcept
    {
        auto e = curl_multi_setopt(raw, CURLMOPT_MAX_TOTAL_CONNECTIONS, n);
        if (e)
            return unexpected{error{e}};
        return {};
    }


} // namespace curl
