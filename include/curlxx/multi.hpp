/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_MULTI_HPP
#define CURLXX_MULTI_HPP

#include <cstddef>
#include <expected>
#include <vector>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {

    class easy;


    struct multi : detail::basic_wrapper<CURLM*> {

        using base_type = detail::basic_wrapper<CURLM*>;


        /// Default constructor.
        multi();

        /// Empty constructor.
        constexpr
        multi(std::nullptr_t)
            noexcept
        {}

        explicit
        multi(CURLM* handle);

        /// Move constructor.
        multi(multi&& other)
            noexcept = default;

        /// Move assignment.
        multi&
        operator =(multi&& other)
            noexcept = default;

        /// Destructor.
        ~multi()
            noexcept;


        void
        create();

        void
        create(CURLM* handle);


        void
        destroy()
            noexcept override;


        void
        add(easy& ez);

        std::expected<void, error>
        try_add(easy& ez)
            noexcept;


        void
        remove(easy& ez);

        std::expected<void, error>
        try_remove(easy& ez)
            noexcept;


        unsigned
        perform();

        std::expected<unsigned, error>
        try_perform()
            noexcept;


        struct msg_done {
            easy* handle;
            CURLcode result;
        };

        // Corresponds to curl_multi_info_read()
        std::vector<msg_done>
        get_done();


        /* ------------------------ */
        /* Start of option setters. */
        /* ------------------------ */


        // CURLMOPT_MAXCONNECTS
        // Size of connection cache.

        void
        set_max_connections(long n);

        std::expected<void, error>
        try_set_max_connections(long n)
            noexcept;


        // CURLMOPT_MAX_CONCURRENT_STREAMS
        // Max concurrent streams for http2.

        void
        set_max_concurrent_streams(long n);

        std::expected<void, error>
        try_set_max_concurrent_streams(long n)
            noexcept;


        // CURLMOPT_MAX_HOST_CONNECTIONS
        // Max number of connections to a single host.

        void
        set_max_host_connections(long n);

        std::expected<void, error>
        try_set_max_host_connections(long n)
            noexcept;


        // CURLMOPT_MAX_TOTAL_CONNECTIONS
        // Max simultaneously open connections.

        void
        set_max_total_connections(long n);

        std::expected<void, error>
        try_set_max_total_connections(long n)
            noexcept;


#if CURL_AT_LEAST_VERSION(8, 16, 0)

        // CURLMOPT_NETWORK_CHANGED
        // Signal that the network has changed.

        void
        set_network_changed(long mask);

        std::expected<void, error>
        try_set_network_changed(long mask)
            noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


        // CURLMOPT_NOTIFYDATA
        // Custom pointer passed to the notify callback. TODO

        // CURLMOPT_NOTIFYFUNCTION
        // Callback that receives notifications. TODO


        // CURLMOPT_PIPELINING
        // Enable HTTP multiplexing.

        void
        set_pipelining(long mask);

        std::expected<void, error>
        try_set_pipelining(long mask)
            noexcept;


        // CURLMOPT_PUSHDATA
        // Pointer to pass to push callback. TODO

        // CURLMOPT_PUSHFUNCTION
        // Callback that approves or denies server pushes. TODO

        // CURLMOPT_SOCKETDATA
        // Custom pointer passed to the socket callback. TODO

        // CURLMOPT_SOCKETFUNCTION
        // Callback informed about what to wait for. TODO

        // CURLMOPT_TIMERDATA
        // Custom pointer to pass to timer callback. TODO

        // CURLMOPT_TIMERFUNCTION
        // Callback to receive timeout values. TODO


        /* ---------------------- */
        /* End of option setters. */
        /* ---------------------- */


    private:

        template<typename T>
        std::expected<void, error>
        try_setopt(CURLMoption opt, T&& arg)
            noexcept;

    };

} // namespace curl

#endif
