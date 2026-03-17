/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_SLIST_HPP
#define CURLXX_SLIST_HPP

#include <expected>
#include <string>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {

    struct slist : detail::basic_wrapper<curl_slist*> {

        using base_type = detail::basic_wrapper<curl_slist*>;


        // Inherit constructors.
        using base_type::base_type;


        /// Move constructor.
        slist(slist&& other)
            noexcept = default;

        /// Move assignment.
        slist&
        operator =(slist&& other)
            noexcept = default;

        /// Destructor.
        ~slist()
            noexcept;


        void
        destroy()
            noexcept;


        void
        append(const std::string& str);

        std::expected<void, error>
        try_append(const std::string& str)
            noexcept;

    }; // struct slist

} // namespace curl

#endif
