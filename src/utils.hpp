/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_SRC_UTILS_HPP
#define CURLXX_SRC_UTILS_HPP

#include <concepts>
#include <expected>
#include <utility>


namespace curl::utils {

    namespace concepts {

        template<typename T>
        concept not_void = !std::same_as<T, void>;

    } // namespace concepts


    template<typename E>
    void
    value_or_throw(const std::expected<void, E>& arg)
    {
        if (!arg)
            throw arg.error();
    }


    template<concepts::not_void T,
             typename E>
    T
    value_or_throw(const std::expected<T, E>& arg)
    {
        if (!arg)
            throw arg.error();
        return *arg;
    }


    template<concepts::not_void T,
             typename E>
    T
    value_or_throw(std::expected<T, E>&& arg)
    {
        if (!arg)
            throw arg.error();
        return std::move(*arg);
    }

} // namespace curl::utils

#endif
