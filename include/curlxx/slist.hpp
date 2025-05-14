/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_SLIST_HPP
#define CURLXX_SLIST_HPP

#include <string>

#include <curl/curl.h>

#include "basic_wrapper.hpp"


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


        ~slist()
            noexcept;


        void
        destroy()
            noexcept;


        void
        append(const std::string& str);

    };

} // namespace curl

#endif
