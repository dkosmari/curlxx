/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_HEADER_HPP
#define CURLXX_HEADER_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>

#include <curl/curl.h>


namespace curl {

    struct header {

        std::string name{};
        std::string value{};
        std::size_t amount{};
        std::size_t index{};
        unsigned origin{};

        constexpr
        header()
            noexcept = default;

        header(const curl_header* h);

    }; // struct header


    class header_iterator {

        curl_header* current = nullptr;
        CURL* raw_easy = nullptr;
        unsigned origin = 0;
        int request = -1;
        header data;


        void
        update_data();


    public:

        using difference_type = std::ptrdiff_t;
        using value_type      = header;
        using pointer         = const value_type*;
        using reference       = const value_type&;

        using iterator_category = std::input_iterator_tag;
        using iterator_concept  = std::input_iterator_tag;


        header_iterator()
            noexcept;

        header_iterator(CURL* raw_easy_,
                        unsigned origin_ = CURLH_HEADER,
                        int request_ = -1);


        bool
        operator ==(const header_iterator& other)
            const noexcept;


        header_iterator
        begin()
            const noexcept;


        header_iterator
        end()
            const noexcept;


        header_iterator&
        operator ++();


        header_iterator
        operator ++(int);


        reference
        operator *()
            const noexcept;


        pointer
        operator ->()
            const noexcept;

    }; // class header_iterator

} // namespace curl

#endif
