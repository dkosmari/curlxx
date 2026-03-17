/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/multi.hpp"

#include "curlxx/easy.hpp"
#include "utils.hpp"


using std::expected;
using std::unexpected;

using curl::utils::value_or_throw;


namespace curl {

    multi::multi()
    {
        create();
    }


    multi::multi(CURLM* handle)
    {
        create(handle);
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
    multi::create(CURLM* handle)
    {
        destroy();
        acquire(handle);
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
        return value_or_throw(try_add(ez));
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
        return value_or_throw(try_remove(ez));
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
        return value_or_throw(try_perform());
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
        return value_or_throw(try_set_max_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_connections(long n)
        noexcept
    {
        return try_setopt(CURLMOPT_MAXCONNECTS, n);
    }


    void
    multi::set_max_concurrent_streams(long n)
    {
        return value_or_throw(try_set_max_concurrent_streams(n));
    }


    std::expected<void, error>
    multi::try_set_max_concurrent_streams(long n)
        noexcept
    {
        return try_setopt(CURLMOPT_MAX_CONCURRENT_STREAMS, n);
    }


    void
    multi::set_max_host_connections(long n)
    {
        return value_or_throw(try_set_max_host_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_host_connections(long n)
        noexcept
    {
        return try_setopt(CURLMOPT_MAX_HOST_CONNECTIONS, n);
    }


    void
    multi::set_max_total_connections(long n)
    {
        return value_or_throw(try_set_max_total_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_total_connections(long n)
        noexcept
    {
        return try_setopt(CURLMOPT_MAX_TOTAL_CONNECTIONS, n);
    }


#if CURL_AT_LEAST_VERSION(8, 16, 0)

    void
    multi::set_network_changed(long mask)
    {
        return value_or_throw(try_set_network_changed(mask));
    }


    std::expected<void, error>
    multi::try_set_network_changed(long mask)
        noexcept
    {
        return try_setopt(CURLMOPT_NETWORK_CHANGED, mask);
    }

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


    void
    multi::set_pipelining(long mask)
    {
        return value_or_throw(try_set_pipelining(mask));
    }


    std::expected<void, error>
    multi::try_set_pipelining(long mask)
        noexcept
    {
        return try_setopt(CURLMOPT_PIPELINING, mask);
    }


    template<typename T>
    std::expected<void, error>
    multi::try_setopt(CURLMoption opt, T&& arg)
        noexcept
    {
        auto e = curl_multi_setopt(raw, opt, arg);
        if (e != CURLM_OK)
            return std::unexpected{error{e}};
        return {};
    }

} // namespace curl
