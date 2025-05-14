/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_HEADER_HPP
#define CURLXX_HEADER_HPP

#include <cstdint>
#include <string>

#include <curl/curl.h>


namespace curl {

    struct header {

        std::string name;
        std::string value;
        std::size_t amount;
        std::size_t index;
        unsigned origin;

        constexpr
        header()
            noexcept = default;

        header(const curl_header* h);

    };

} // namespace curl

#endif
