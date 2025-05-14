/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_MULTI_HPP
#define CURLXX_MULTI_HPP

#include <expected>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {

    class easy;


    struct multi : detail::basic_wrapper<CURLM*> {

        using base_type = detail::basic_wrapper<CURLM*>;


        // Inherit constructors.
        using base_type::base_type;


        multi();

        multi(multi&& other)
            noexcept = default;

        multi&
        operator =(multi&& other)
            noexcept = default;


        ~multi() noexcept;


        void
        create();


        void
        destroy()
            noexcept override;


        void
        add(easy& ez);

        std::expected<void, error>
        try_add(easy& ez)
            noexcept;


        void
        remove(easy& ez);

        std::expected<void, error>
        try_remove(easy& ez)
            noexcept;


        unsigned
        perform();

        std::expected<unsigned, error>
        try_perform()
            noexcept;

    };


} // namespace curl

#endif
