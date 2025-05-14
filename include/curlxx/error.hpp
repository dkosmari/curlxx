/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_ERROR_HPP
#define CURLXX_ERROR_HPP

#include <stdexcept>
#include <string>

#include <curl/curl.h>


namespace curl {


    std::string
    to_string(CURLcode code);


    std::string
    to_string(CURLMcode code);


    std::string
    to_string(CURLHcode code);



    struct error : std::runtime_error {

        error(const char* msg);

        error(const std::string& msg);

        error(CURLcode code);

        error(CURLMcode code);

        error(CURLHcode code);

    };

} // namespace curl

#endif
