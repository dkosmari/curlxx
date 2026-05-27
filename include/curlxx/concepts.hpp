/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <string>
#include <string_view>
#include <type_traits>


namespace curl::concepts {

    namespace details {

        template<typename T>
        struct is_char_like_helper
            : std::false_type {};

        template<>
        struct is_char_like_helper<char>
            : std::true_type {};

        template<>
        struct is_char_like_helper<wchar_t>
            : std::true_type {};

        template<>
        struct is_char_like_helper<char8_t>
            : std::true_type {};

        template<>
        struct is_char_like_helper<char16_t>
            : std::true_type {};

        template<>
        struct is_char_like_helper<char32_t>
            : std::true_type {};


        template<typename T>
        using is_char_like = is_char_like_helper<std::remove_cvref_t<T>>;


        template<typename T>
        inline constexpr const
        bool is_char_like_v = is_char_like<T>::value;

    }

    template<typename T>
    concept char_like = details::is_char_like_v<T>;


    namespace details {

        template<typename T>
        struct is_string_like_helper
            : std::false_type {};

        template<char_like CharT>
        struct is_string_like_helper<CharT*>
            : std::true_type {};

        template<typename CharT,
                 typename Traits,
                 typename Allocator>
        struct is_string_like_helper<std::basic_string<CharT, Traits, Allocator>>
            : std::true_type {};

        template<typename CharT,
                 typename Traits>
        struct is_string_like_helper<std::basic_string_view<CharT, Traits>>
            : std::true_type {};


        template<typename T>
        using is_string_like = is_string_like_helper<std::decay_t<std::remove_cvref_t<T>>>;


        template<typename T>
        inline constexpr const
        bool is_string_like_v = is_string_like<T>::value;

    } // namespace details

    template<typename T>
    concept string_like = details::is_string_like_v<T>;

} // namespace curl::concepts
