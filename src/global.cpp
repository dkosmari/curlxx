/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/global.hpp"

#include "curlxx/error.hpp"


namespace curl::global {

    void
    init::cleanup()
        noexcept
    {
        if (initialized) {
            curl_global_cleanup();
            initialized = false;
        }
    }


    init::init(long flags)
    {
        auto e = curl_global_init(flags);
        if (e)
            throw error{e};
        initialized = true;
    }



    init::init(long flags,
               curl_malloc_callback malloc_cb,
               curl_free_callback free_cb,
               curl_realloc_callback realloc_cb,
               curl_strdup_callback strdup_cb,
               curl_calloc_callback calloc_cb)
    {
        auto e = curl_global_init_mem(flags,
                                      malloc_cb,
                                      free_cb,
                                      realloc_cb,
                                      strdup_cb,
                                      calloc_cb);
        if (e)
            throw error{e};
        initialized = true;
    }


    init::~init()
        noexcept
    {
        cleanup();
    }


    init::init(init&& other)
        noexcept :
        initialized{other.initialized}
    {
        other.initialized = false;
    }


    init&
    init::operator =(init&& other)
        noexcept
    {
        if (this != &other) {
            if (initialized)
                cleanup();
            initialized = other.initialized;
            other.initialized = false;
        }
        return *this;
    }


    void
    set_ssl_backend(curl_sslbackend id)
    {
        auto e = curl_global_sslset(id, nullptr, nullptr);
        if (e)
            throw error{e};
    }


    void
    set_ssl_backend(const std::string& name)
    {
        auto e = curl_global_sslset(CURLSSLBACKEND_NONE, name.data(), nullptr);
        if (e)
            throw error{e};
    }


    std::vector<ssl_backend>
    get_ssl_backends()
    {
        std::vector<ssl_backend> result;
        const curl_ssl_backend** backend_list = nullptr;
        curl_global_sslset(CURLSSLBACKEND_NONE, nullptr, &backend_list);
        for (unsigned i = 0; backend_list[i]; ++i)
            result.emplace_back(backend_list[i]->id, backend_list[i]->name);
        return result;
    }


#if CURL_AT_LEAST_VERSION(8, 3, 0)

    void
    set_trace(const std::string& cfg)
    {
        auto e = curl_global_trace(cfg.data());
        if (e)
            throw error{e};
    }

#endif

} // namespace curl::global
