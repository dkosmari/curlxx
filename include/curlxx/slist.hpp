/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_SLIST_HPP
#define CURLXX_SLIST_HPP

#include <cstddef>
#include <expected>
#include <initializer_list>
#include <iterator>
#include <string>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {

    struct slist : detail::basic_wrapper<curl_slist*> {

        using base_type = detail::basic_wrapper<curl_slist*>;
        using value_type = const char*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;


        struct const_iterator {

            using difference_type = std::ptrdiff_t;
            using value_type      = const char*;
            using reference       = value_type&;
            using const_reference = const value_type&;


            constexpr
            const_iterator()
                noexcept = default;


            explicit
            constexpr
            const_iterator(const curl_slist* node)
                noexcept :
                ptr{node}
            {}


            constexpr
            const_reference
            operator *()
                const noexcept
            {
                return ptr->data;
            }


            constexpr
            const_iterator&
            operator ++()
                noexcept
            {
                ptr = ptr->next;
                return *this;
            }


            const_iterator
            operator ++(int)
                noexcept
            {
                const_iterator old{ptr};
                ptr = ptr->next;
                return old;
            }


            constexpr
            bool
            operator ==(const const_iterator& other)
                const noexcept = default;

        private:

            const curl_slist* ptr = nullptr;

        }; // struct const_iterator

        using iterator = const_iterator;


        // Inherit constructors.
        using base_type::base_type;


        slist(std::initializer_list<value_type> values);

        slist(std::initializer_list<std::string> values);


        template<std::input_iterator It>
        slist(It start,
              It finish)
        {
            for (It it = start; it != finish; ++it)
                append(*it);
        }


        /// Copy constructor.
        slist(const slist& other);

        /// Move constructor.
        slist(slist&& other)
            noexcept = default;

        /// Copy assignment.
        slist&
        operator =(const slist& other);

        /// Move assignment.
        slist&
        operator =(slist&& other)
            noexcept = default;

        /// Destructor.
        ~slist()
            noexcept;


        void
        destroy()
            noexcept;


        void
        append(value_type str);

        void
        append(const std::string& str);


        std::expected<void, error>
        try_append(value_type str)
            noexcept;

        std::expected<void, error>
        try_append(const std::string& str)
            noexcept;


        const_iterator
        begin()
            const noexcept;

        const_iterator
        cbegin()
            const noexcept;


        const_iterator
        end()
            const noexcept;

        const_iterator
        cend()
            const noexcept;


        size_type
        size()
            const noexcept;


        constexpr
        bool
        empty()
            const noexcept
        {
            return !raw;
        }

      }; // struct slist

} // namespace curl

#endif
