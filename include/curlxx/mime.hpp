/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_MIME_HPP
#define CURLXX_MIME_HPP

#include <cstddef>
#include <expected>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <curl/curl.h>

#include "error.hpp"
#include "owner_wrapper.hpp"


namespace curl {

    class easy;


    struct mime : detail::owner_wrapper<curl_mime*> {

        using base_type = detail::owner_wrapper<curl_mime*>;

        using base_type::raw_type;

        using base_type::state_type;


        // Note: this is a non-owning object, its lifetime is tied to the mime object.
        struct part {

            using read_callback_t = std::size_t (char* buffer,
                                                 std::size_t element_size,
                                                 std::size_t num_elements);

            using seek_callback_t = int (curl_off_t offset,
                                         int origin);

            using free_callback_t = void ();

            using read_func_t = std::function<read_callback_t>;
            using seek_func_t = std::function<seek_callback_t>;
            using free_func_t = std::function<free_callback_t>;


            const curl_mimepart*
            data()
                const noexcept;

            curl_mimepart*
            data()
                noexcept;


            // wrap curl_mime_data()

            void
            set_data(const void* data,
                     std::size_t size);

            void
            set_data(const std::string& data);

            void
            set_data(std::string_view data);

            template<typename T,
                     std::size_t E>
            inline
            void
            set_data(std::span<T, E> data)
            {
                set_data(data.data(), data.size_bytes());
            }


            std::expected<void, error>
            try_set_data(const void* data,
                         std::size_t size)
                noexcept;

            std::expected<void, error>
            try_set_data(const std::string& data)
                noexcept;

            std::expected<void, error>
            try_set_data(std::string_view data)
                noexcept;

            template<typename T,
                     std::size_t E>
            inline
            std::expected<void, error>
            try_set_data(std::span<T, E> data)
                noexcept
            {
                return try_set_data(data.data(), data.size_bytes());
            }


            // wrap curl_mime_data_cb()

            void
            set_data_callbacks(curl_off_t size,
                               read_func_t read_func,
                               seek_func_t seek_func = {},
                               free_func_t free_func = {});

            std::expected<void, error>
            try_set_data_callbacks(curl_off_t size,
                                   read_func_t read_func,
                                   seek_func_t seek_func = {},
                                   free_func_t free_func = {})
                noexcept;


            // wrap curl_mime_encoder()

            void
            set_encoder(const std::string& encoder);

            std::expected<void, error>
            try_set_encoder(const std::string& encoder)
                noexcept;

            void
            unset_encoder()
                noexcept;


            // wrap curl_mime_filedata()

            void
            set_file_data(const std::filesystem::path& filename);

            std::expected<void, error>
            try_set_file_data(const std::filesystem::path& filename)
                noexcept;

            void
            unset_file_data()
                noexcept;


            // wrap curl_mime_filename()

            void
            set_file_name(std::filesystem::path& remote_filename);

            std::expected<void, error>
            try_set_file_name(std::filesystem::path& remote_filename)
                noexcept;

            void
            unset_file_name()
                noexcept;


            // wrap curl_mime_headers()

            void
            set_header(const std::vector<std::string>& headers);

            std::expected<void, error>
            try_set_headers(const std::vector<std::string>& headers)
                noexcept;

            void
            unset_headers()
                noexcept;


            // wrap curl_mime_name()

            void
            set_name(const std::string& name);

            std::expected<void, error>
            try_set_name(const std::string& name)
                noexcept;

            void
            unset_name()
                noexcept;


            // wrap curl_mime_subparts()

            void
            set_subparts(mime& subparts);

            std::expected<void, error>
            try_set_subparts(mime& subparts)
                noexcept;

            void
            unset_subparts()
                noexcept;


            // wrap curl_mime_type()

            void
            set_mime_type(const std::string& mime_type);

            std::expected<void, error>
            try_set_mime_type(const std::string& mime_type)
                noexcept;

            void
            unset_mime_type()
                noexcept;


        private:

            curl_mimepart* raw = nullptr;

            part(mime* parent);

            friend class mime;

            struct context {

                read_func_t read_func;
                seek_func_t seek_func;
                free_func_t free_func;

                static
                std::size_t
                read_helper(char* buffer,
                            std::size_t element_size,
                            std::size_t num_elements,
                            void* arg);

                static
                int
                seek_helper(void* arg,
                            curl_off_t offset,
                            int origin);

                static
                void
                free_helper(void *arg);

            }; // struct context

        }; // struct part



        mime();

        mime(easy& ez);

        mime(mime&& other)
            noexcept;

        mime&
        operator =(mime&& other)
            noexcept;

        ~mime()
            noexcept;


        void
        create();

        void
        create(easy& ez);

        void
        destroy()
            noexcept override;

        part
        add_part();

    }; // struct mime

} // namespace curl

#endif
