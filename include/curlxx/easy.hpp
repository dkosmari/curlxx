/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_EASY_HPP
#define CURLXX_EASY_HPP

#include <expected>
#include <functional>
#include <span>
#include <string>
#include <tuple>
#include <vector>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"
#include "header.hpp"
#include "slist.hpp"


namespace curl {

    class multi;


    class easy : public detail::basic_wrapper<CURL*> {

    public:

        using base_type = detail::basic_wrapper<CURL*>;

        using read_function_t = std::size_t(std::span<char>);
        using write_function_t = std::size_t(std::span<const char>);

        struct extra_state_type {
            std::vector<char> error_buffer;
            std::function<read_function_t> read_callback;
            std::function<write_function_t> write_callback;
            slist header_list;
        };

        using state_type = std::tuple<raw_type, extra_state_type>;


    private:

        extra_state_type state;


        void init_error_buffer();
        void link_this();

        static
        std::size_t
        dispatch_read_callback(char* buffer,
                               std::size_t,
                               std::size_t size,
                               CURL* handle)
            noexcept;

        static
        std::size_t
        dispatch_write_callback(const char* buffer,
                                std::size_t,
                                std::size_t size,
                                CURL* handle)
            noexcept;


    public:

        // Inherit constructors.
        using base_type::base_type;


        easy();

        easy(const easy& other);

        easy(easy&& other)
            noexcept;

        easy&
        operator =(const easy& other);

        easy&
        operator =(easy&& other)
            noexcept;


        ~easy()
            noexcept;


        void
        create();

        void
        create(const easy& other);


        void
        destroy()
            noexcept override;


        [[nodiscard]]
        state_type
        release()
            noexcept;


        void
        acquire(state_type new_state)
            noexcept;

        void
        acquire(raw_type new_raw);


        void
        reset();


        void
        pause(bool pause_recv = true,
              bool pause_send = true);

        std::expected<void, error>
        try_pause(bool pause_recv,
                  bool pause_send)
            noexcept;

        void
        unpause();


        void
        perform();

        std::expected<void, error>
        try_perform()
            noexcept;


        void
        set_follow(bool enable = true);

        std::expected<void, error>
        try_set_follow(bool enable = true)
            noexcept;


        void
        set_forbid_reuse(bool forbid = true);

        std::expected<void, error>
        try_set_forbid_reuse(bool forbid = true)
            noexcept;


        void
        set_http_headers(const std::vector<std::string>& headers);

        std::expected<void, error>
        try_set_http_headers(const std::vector<std::string>& headers)
            noexcept;


        void
        set_read_function(std::function<read_function_t> fn);

        std::expected<void, error>
        try_set_read_function(std::function<read_function_t> fn)
            noexcept;


        void
        set_ssl_verify_peer(bool enabled);

        std::expected<void, error>
        try_set_ssl_verify_peer(bool enabled)
            noexcept;


        void
        set_url(const std::string& url);

        std::expected<void, error>
        try_set_url(const std::string& url)
            noexcept;

        void
        set_url();

        std::expected<void, error>
        try_set_url()
            noexcept;


        void
        set_user_agent(const std::string& ua);

        std::expected<void, error>
        try_set_user_agent(const std::string& ua)
            noexcept;


        void
        set_verbose(bool v = true)
            noexcept;


        void
        set_write_function(std::function<write_function_t> fn);

        std::expected<void, error>
        try_set_write_function(std::function<write_function_t> fn)
            noexcept;


        header
        get_header(const std::string& name,
                   std::size_t index = 0,
                   unsigned origin = CURLH_HEADER,
                   int request = -1);


        std::expected<header, error>
        try_get_header(const std::string& name,
                       std::size_t index = 0,
                       unsigned origin = CURLH_HEADER,
                       int request = -1) noexcept;


        [[nodiscard]]
        static
        easy*
        get_wrapper(CURL* h)
            noexcept;


        friend class multi;

    }; // class easy

} // namespace curl

#endif
