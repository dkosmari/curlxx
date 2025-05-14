/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/header.hpp"

namespace curl {

    header::header(const curl_header* h) :
        name{h->name},
        value{h->value},
        amount{h->amount},
        index{h->index},
        origin{h->origin}
    {}

} // namespace curl
