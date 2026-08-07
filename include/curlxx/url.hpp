/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_URL_HPP
#define CURLXX_URL_HPP

#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {


    class url : public detail::basic_wrapper<CURLU*> {

    public:

        /*--------------*/
        /* Type aliases */
        /*--------------*/

        using base_type = detail::basic_wrapper<CURLU*>;


        /// Default constructor.
        url();

        /// Empty constructor.
        inline
        url(std::nullptr_t)
            noexcept
        {}

        /// Copy constructor.
        url(const url& other);

        /// Move constructor.
        url(url&& other)
            noexcept;

        /// Copy assignment.
        url&
        operator =(const url& other);

        /// Move assignment.
        url&
        operator =(url&& other)
            noexcept;

        /// Destructor.
        ~url()
            noexcept;


        void
        create();

        void
        create(CURLU* handle);

        void
        create(const url& other);

        void
        destroy()
            noexcept override;


        /*---------*/
        /* Getters */
        /*---------*/

        // CURLUPART_URL

        std::string
        get_url(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_url(unsigned flags = 0)
            const noexcept;


        // CURLUPART_USER

        std::string
        get_user(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_user(unsigned flags = 0)
            const noexcept;


        // CURLUPART_PASSWORD

        std::string
        get_password(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_password(unsigned flags = 0)
            const noexcept;


        // CURLUPART_OPTIONS

        std::string
        get_options(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_options(unsigned flags = 0)
            const noexcept;


        // CURLUPART_HOST

        std::string
        get_host(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_host(unsigned flags = 0)
            const noexcept;


        // CURLUPART_ZONEID

        std::string
        get_zone_id(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_zone_id(unsigned flags = 0)
            const noexcept;


        // CURLUPART_PORT

        std::string
        get_port(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_port(unsigned flags = 0)
            const noexcept;


        // CURLUPART_PATH

        std::string
        get_path(unsigned flags = 0)
            const;

        std::expected<std::string, error>
        try_get_path(unsigned flags = 0)
            const noexcept;


        // CURLUPART_QUERY

        std::optional<std::string>
        get_query(unsigned flags = 0)
            const;

        std::expected<std::optional<std::string>, error>
        try_get_query(unsigned flags = 0)
            const noexcept;


        // CURLUPART_FRAGMENT

        std::optional<std::string>
        get_fragment(unsigned flags = 0)
            const;

        std::expected<std::optional<std::string>, error>
        try_get_fragment(unsigned flags = 0)
            const noexcept;


        /*---------*/
        /* Setters */
        /*---------*/

        // CURLUPART_URL

        void
        set_url(const std::string& url_arg,
                unsigned flags = 0);

        std::expected<void, error>
        try_set_url(const std::string url_arg,
                    unsigned flags = 0)
            noexcept;

        void
        unset_url(unsigned flags = 0)
            noexcept;


        // CURLUPART_SCHEME

        void
        set_scheme(const std::string& scheme,
                   unsigned flags = 0);

        std::expected<void, error>
        try_set_scheme(const std::string& scheme,
                       unsigned flags = 0)
            noexcept;

        void
        unset_scheme(unsigned flags = 0)
            noexcept;


        // CURLUPART_USER

        void
        set_user(const std::string& user,
                 unsigned flags = 0);

        std::expected<void, error>
        try_set_user(const std::string& user,
                     unsigned flags = 0)
            noexcept;

        void
        unset_user(unsigned flags = 0)
            noexcept;


        // CURLUPART_PASSWORD

        void
        set_password(const std::string& password,
                     unsigned flags = 0);

        std::expected<void, error>
        try_set_password(const std::string& password,
                         unsigned flags = 0)
            noexcept;

        void
        unset_password(unsigned flags = 0)
            noexcept;


        // CURLUPART_OPTIONS

        void
        set_options(const std::string& options,
                    unsigned flags = 0);

        std::expected<void, error>
        try_set_options(const std::string& options,
                        unsigned flags = 0)
            noexcept;

        void
        unset_options(unsigned flags = 0)
            noexcept;


        // CURLUPART_HOST

        void
        set_host(const std::string& host,
                 unsigned flags = 0);

        std::expected<void, error>
        try_set_host(const std::string& host,
                     unsigned flags = 0)
            noexcept;

        void
        unset_host(unsigned flags = 0)
            noexcept;


        // CURLUPART_ZONEID

        void
        set_zone_id(const std::string& zone_id,
                    unsigned flags = 0);

        std::expected<void, error>
        try_set_zone_id(const std::string& zone_id,
                        unsigned flags = 0)
            noexcept;

        void
        unset_zone_id(unsigned flags = 0)
            noexcept;


        // CURLUPART_PORT

        void
        set_port(const std::string& port,
                 unsigned flags = 0);

        std::expected<void, error>
        try_set_port(const std::string& port,
                     unsigned flags = 0)
            noexcept;

        void
        set_port(std::uint16_t port,
                 unsigned flags = 0);

        std::expected<void, error>
        try_set_port(std::uint16_t port,
                     unsigned flags = 0)
            noexcept;

        void
        unset_port(unsigned flags = 0)
            noexcept;


        // CURLUPART_PATH

        void
        set_path(const std::string& path,
                 unsigned flags = 0);

        std::expected<void, error>
        try_set_path(const std::string& path,
                     unsigned flags = 0)
            noexcept;

        void
        unset_path(unsigned flags = 0)
            noexcept;


        // CURLUPART_QUERY

        void
        set_query(const std::string& query,
                  unsigned flags = 0);

        std::expected<void, error>
        try_set_query(const std::string& query,
                      unsigned flags = 0)
            noexcept;

        void
        unset_query(unsigned flags = 0)
            noexcept;


        // CURLUPART_FRAGMENT

        void
        set_fragment(const std::string& fragment,
                     unsigned flags = 0);

        std::expected<void, error>
        try_set_fragment(const std::string& fragment,
                         unsigned flags = 0)
            noexcept;

        void
        unset_fragment(unsigned flags = 0)
            noexcept;

    }; // class url

} // namespace curl

#endif
