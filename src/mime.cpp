/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */


#include <new>
#include <stdexcept>
#include <utility>

#include "curlxx/mime.hpp"

#include "curlxx/easy.hpp"
#include "curlxx/slist.hpp"


namespace curl {


    std::size_t
    mime::part::context::read_helper(char* buffer,
                                     std::size_t element_size,
                                     std::size_t num_elements,
                                     void* arg)
    {
        try {
            auto ctx = reinterpret_cast<context*>(arg);
            if (ctx->read_func)
                return ctx->read_func(buffer,
                                      element_size,
                                      num_elements);
            return 0;
        }
        catch (...) {
            return CURL_READFUNC_ABORT;
        }
    }


    int
    mime::part::context::seek_helper(void* arg,
                                     curl_off_t offset,
                                     int origin)
    {
        try {
            auto ctx = reinterpret_cast<context*>(arg);
            if (ctx->seek_func)
                return ctx->seek_func(offset, origin);
            return CURL_SEEKFUNC_CANTSEEK;
        }
        catch (...) {
            return CURL_SEEKFUNC_FAIL;
        }
    }


    void
    mime::part::context::free_helper(void *arg)
    {
        try {
            auto ctx = reinterpret_cast<context*>(arg);
            if (ctx->free_func)
                ctx->free_func();
            delete ctx;
        }
        catch (...) {}
    }


    const curl_mimepart*
    mime::part::data()
        const noexcept
    {
        return raw;
    }


    curl_mimepart*
    mime::part::data()
        noexcept
    {
        return raw;
    }


    mime::part::part(mime* parent) :
        raw{curl_mime_addpart(parent->data())}
    {
        if (!raw)
            throw error{"curl_mime_addpart() failed"};
    }


    void
    mime::part::set_data(const void* data,
                         std::size_t size)
    {
        auto result = try_set_data(data, size);
        if (!result)
            throw result.error();
    }


    void
    mime::part::set_data(const std::string& data)
    {
        auto result = try_set_data(data);
        if (!result)
            throw result.error();
    }


    void
    mime::part::set_data(std::string_view data)
    {
        auto result = try_set_data(data);
        if (!result)
            throw result.error();
    }



    std::expected<void, error>
    mime::part::try_set_data(const void* data,
                             std::size_t size)
        noexcept
    {
        auto e = curl_mime_data(raw, reinterpret_cast<const char*>(data), size);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    std::expected<void, error>
    mime::part::try_set_data(const std::string& data)
        noexcept
    {
        auto e = curl_mime_data(raw, data.data(), data.size());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    std::expected<void, error>
    mime::part::try_set_data(std::string_view data)
        noexcept
    {
        auto e = curl_mime_data(raw, data.data(), data.size());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::set_data_callbacks(curl_off_t size,
                                   std::function<read_callback_t> read_cb,
                                   std::function<seek_callback_t> seek_cb,
                                   std::function<free_callback_t> free_cb)
    {
        auto result = try_set_data_callbacks(size,
                                             std::move(read_cb),
                                             std::move(seek_cb),
                                             std::move(free_cb));
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_data_callbacks(curl_off_t size,
                                       std::function<read_callback_t> read_func,
                                       std::function<seek_callback_t> seek_func,
                                       std::function<free_callback_t> free_func)
        noexcept
    {
        try {
            auto ctx = new(std::nothrow) context{
                std::move(read_func),
                std::move(seek_func),
                std::move(free_func)
            };
            if (!ctx)
                return std::unexpected{error{CURLE_OUT_OF_MEMORY}};

            auto e = curl_mime_data_cb(raw,
                                       size,
                                       context::read_helper,
                                       context::seek_helper,
                                       context::free_helper,
                                       ctx);
            if (e) {
                delete ctx;
                return std::unexpected{error{e}};
            }
            return {};
        }
        catch (...) {
            return std::unexpected{error{CURLE_OUT_OF_MEMORY}};
        }
    }


    void
    mime::part::set_encoder(const std::string& encoder)
    {
        auto result = try_set_encoder(encoder);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_encoder(const std::string& encoder)
        noexcept
    {
        auto e = curl_mime_encoder(raw, encoder.data());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::unset_encoder()
        noexcept
    {
        curl_mime_encoder(raw, nullptr);
    }


    void
    mime::part::set_file_data(const std::filesystem::path& filename)
    {
        auto result = try_set_file_data(filename);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_file_data(const std::filesystem::path& filename)
        noexcept
    {
        auto e = curl_mime_filedata(raw, filename.c_str());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::unset_file_data()
        noexcept
    {
        curl_mime_filedata(raw, nullptr);
    }


    void
    mime::part::set_file_name(std::filesystem::path& remote_filename)
    {
        auto result = try_set_file_name(remote_filename);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_file_name(std::filesystem::path& remote_filename)
        noexcept
    {
        auto e = curl_mime_filename(raw, remote_filename.c_str());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::unset_file_name()
        noexcept
    {
        curl_mime_filename(raw, nullptr);
    }


    void
    mime::part::set_header(const std::vector<std::string>& headers)
    {
        auto result = try_set_headers(headers);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_headers(const std::vector<std::string>& headers)
        noexcept
    {
        slist hdr_list;
        for (auto& hdr : headers) {
            auto result = hdr_list.try_append(hdr);
            if (!result)
                return result;
        }
        // Note: we transfer ownership of hdr_list's raw pointer.
        auto e = curl_mime_headers(raw, hdr_list.data(), true);
        if (e)
            return std::unexpected{error{e}};
        hdr_list.release();
        return {};
    }


    void
    mime::part::unset_headers()
        noexcept
    {
        curl_mime_headers(raw, nullptr, true);
    }


    void
    mime::part::set_name(const std::string& name)
    {
        auto result = try_set_name(name);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_name(const std::string& name)
        noexcept
    {
        auto e = curl_mime_name(raw, name.data());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::unset_name()
        noexcept
    {
        curl_mime_name(raw, nullptr);
    }


    void
    mime::part::set_subparts(mime& subparts)
    {
        auto result = try_set_subparts(subparts);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_subparts(mime& subparts)
        noexcept
    {
        auto e = curl_mime_subparts(raw, subparts.data());
        if (e)
            return std::unexpected{error{e}};
        // Note: on success, turn subparts into an observer.
        subparts.disown();
        return {};
    }


    void
    mime::part::unset_subparts()
        noexcept
    {
        curl_mime_subparts(raw, nullptr);
    }


    void
    mime::part::set_mime_type(const std::string& mime_type)
    {
        auto result = try_set_mime_type(mime_type);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    mime::part::try_set_mime_type(const std::string& mime_type)
        noexcept
    {
        auto e = curl_mime_type(raw, mime_type.data());
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    mime::part::unset_mime_type()
        noexcept
    {
        curl_mime_type(raw, nullptr);
    }


    mime::mime()
    {
        create();
    }


    mime::mime(easy& ez)
    {
        create(ez);
    }


    mime::mime(mime&& other)
        noexcept = default;


    mime&
    mime::operator =(mime&& other)
        noexcept = default;


    mime::~mime()
        noexcept
    {
        destroy();
    }


    void
    mime::create()
    {
        auto new_raw = curl_mime_init(nullptr);
        if (!new_raw)
            throw error{"curl_mime_init() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    mime::create(easy& ez)
    {
        auto new_raw = curl_mime_init(ez.data());
        if (!new_raw)
            throw error{"curl_mime_init() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    mime::destroy()
        noexcept
    {
        if (is_valid()) {
            auto [old_raw, old_owner] = release();
            curl_mime_free(old_raw);
        }
    }


    mime::part
    mime::add_part()
    {
        return part(this);
    }

} // namespace curl
