/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <curl/curl.h>

#include "curlxx/escape.hpp"

#include "curlxx/error.hpp"


using std::string;


namespace curl {

    string
    escape(const string& input)
    {
        char* s = curl_easy_escape(nullptr, input.data(), input.size());
        if (!s)
            throw error{"failed to escape string"};
        string result = s;
        curl_free(s);
        return result;
    }


    string
    unescape(const string& input)
    {
        int out_size = 0;
        char* s = curl_easy_unescape(nullptr, input.data(), input.size(), &out_size);
        if (!s)
            throw error{"failed to unescape string"};
        string result{s, static_cast<string::size_type>(out_size)};
        curl_free(s);
        return result;
    }

} // namespace curl
