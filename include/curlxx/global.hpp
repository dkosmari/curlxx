/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_GLOBAL_HPP
#define CURLXX_GLOBAL_HPP

#include <string>
#include <vector>

#include <curl/curl.h>


namespace curl::global {

    class init {

        bool initialized = false;

        void
        cleanup()
            noexcept;

    public:

        init(long flags = CURL_GLOBAL_DEFAULT);


        init(long flags,
             curl_malloc_callback malloc_cb,
             curl_free_callback free_cb,
             curl_realloc_callback realloc_cb,
             curl_strdup_callback strdup_cb,
             curl_calloc_callback calloc_cb);


        ~init()
            noexcept;


        // allow moving
        init(init&& other)
            noexcept;

        init&
        operator =(init&& other)
            noexcept;

    }; // struct init


    void
    set_ssl_backend(curl_sslbackend id);

    void
    set_ssl_backend(const std::string& name);


    struct ssl_backend {
        curl_sslbackend id;
        std::string name;
    };

    std::vector<ssl_backend>
    get_ssl_backends();


#if CURL_AT_LEAST_VERSION(8, 3, 0)

    void
    set_trace(const std::string& cfg);

#endif

} // namespace curl::global

#endif
