/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/error.hpp"


using std::string;
using std::runtime_error;


namespace curl {

    string
    to_string(CURLcode code)
    {
        return curl_easy_strerror(code);
    }


    string
    to_string(CURLMcode code)
    {
        return curl_multi_strerror(code);
    }


    string
    to_string(CURLHcode code)
    {
        switch (code) {
            case CURLHE_OK:
                return "no error";
            case CURLHE_BADINDEX:
                return "header exists but not with this index";
            case CURLHE_MISSING:
                return "no such header exists";
            case CURLHE_NOHEADERS:
                return "no headers at all exist";
            case CURLHE_NOREQUEST:
                return "no request with this number was used";
            case CURLHE_OUT_OF_MEMORY:
                return "out of memory while processing";
            case CURLHE_BAD_ARGUMENT:
                return "a function argument was not okay";
            case CURLHE_NOT_BUILT_IN:
                return "HEADER API was disabled in the build";
            default:
                return "invalid";
        }
    }


    error::error(const char* msg) :
        runtime_error{msg}
    {}


    error::error(const string& msg) :
        runtime_error{msg}
    {}


    error::error(CURLcode code) :
        runtime_error{to_string(code)}
    {}


    error::error(CURLMcode code) :
        runtime_error{to_string(code)}
    {}


    error::error(CURLHcode code) :
        runtime_error{to_string(code)}
    {}

} // namespace curl
