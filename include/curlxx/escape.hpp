/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_ESCAPE_HPP
#define CURLXX_ESCAPE_HPP

#include <string>

namespace curl {

    std::string
    escape(const std::string& input);


    std::string
    unescape(const std::string& input);

} // namespace curl

#endif
