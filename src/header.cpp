/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
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


    void
    header_iterator::update_state()
    {
        if (current) {
            data = header{current};
        } else {
            raw_easy = nullptr;
            data = {};
        }
    }


    header_iterator::header_iterator()
        noexcept
    {}


    header_iterator::header_iterator(CURL* raw_easy_,
                                     unsigned origin_,
                                     int request_) :
        raw_easy{raw_easy_},
        origin{origin_},
        request{request_},
        current{curl_easy_nextheader(raw_easy,
                                     origin,
                                     request,
                                     nullptr)}
    {
        update_state();
    }


    bool
    header_iterator::operator ==(const header_iterator& other)
        const noexcept
    {
        return raw_easy == other.raw_easy && current == other.current;
    }


    header_iterator
    header_iterator::begin()
        const noexcept
    {
        return *this;
    }


    header_iterator
    header_iterator::end()
        const noexcept
    {
        return {};
    }


    header_iterator&
    header_iterator::operator ++()
    {
        current = curl_easy_nextheader(raw_easy,
                                       origin,
                                       request,
                                       current);
        update_state();
        return *this;
    }


    header_iterator
    header_iterator::operator ++(int)
    {
        auto old = *this;
        operator ++();
        return old;
    }


    header_iterator::reference
    header_iterator::operator *()
        const noexcept
    {
        return data;
    }


    header_iterator::pointer
    header_iterator::operator ->()
        const noexcept
    {
        return &data;
    }

} // namespace curl
