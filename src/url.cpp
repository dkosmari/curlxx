/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "curlxx/url.hpp"

#include "utils.hpp"


using curl::utils::value_or_throw;


namespace curl {

    namespace {

        // Function declarations.

        std::expected<std::string, error>
        wrap_url_get(CURLU* raw,
                     CURLUPart part,
                     unsigned flags)
            noexcept;


        std::expected<std::optional<std::string>, error>
        wrap_url_get_opt(CURLU* raw,
                         CURLUPart part,
                         unsigned flags)
            noexcept;

        std::expected<void, error>
        wrap_url_set(CURLU* raw,
                     CURLUPart part,
                     const std::string& arg,
                     unsigned flags)
            noexcept;

        void
        wrap_url_unset(CURLU* raw,
                       CURLUPart part,
                       unsigned flags)
            noexcept;


        // Function definitions.

        std::expected<std::string, error>
        wrap_url_get(CURLU* raw,
                     CURLUPart part,
                     unsigned flags)
            noexcept
        {
            char* content = nullptr;
            auto e = curl_url_get(raw, part, &content, flags);
            if (e)
                return std::unexpected{error{e}};
            try {
                if (!content)
                    return std::unexpected{error{"curl_url_get() returned null pointer"}};
                std::string result{content};
                curl_free(content);
                return result;
            }
            catch (...) {
                curl_free(content);
                return std::unexpected{error{"out of memory"}};
            }
        }


        std::expected<std::optional<std::string>, error>
        wrap_url_get_opt(CURLU* raw,
                         CURLUPart part,
                         unsigned flags)
            noexcept
        {
            char* content = nullptr;
            auto e = curl_url_get(raw, part, &content, flags);
            if (e)
                return std::unexpected{error{e}};
            try {
                if (!content)
                    return std::optional<std::string>{};
                std::string result{content};
                curl_free(content);
                return std::optional<std::string>{std::move(result)};
            }
            catch (...) {
                curl_free(content);
                return std::unexpected{error{"out of memory"}};
            }
        }


        std::expected<void, error>
        wrap_url_set(CURLU* raw,
                     CURLUPart part,
                     const std::string& arg,
                     unsigned flags)
            noexcept
        {
            auto e = curl_url_set(raw, part, arg.data(), flags);
            if (e)
                return std::unexpected{error{e}};
            return {};
        }


        void
        wrap_url_unset(CURLU* raw,
                       CURLUPart part,
                       unsigned flags)
            noexcept
        {
            curl_url_set(raw, part, nullptr, flags);
        }

    } // namespace


    // Public functions

    url::url()
    {
        create();
    }


    url::url(const url& other) :
        base_type{}
    {
        create(other);
    }


    url::url(url&& other)
        noexcept
    {
        acquire(other.release());
    }


    url&
    url::operator =(const url& other)
    {
        create(other);
        return *this;
    }


    url&
    url::operator =(url&& other)
        noexcept
    {
        if (this != &other) {
            destroy();
            acquire(other.release());
        }
        return *this;
    }


    url::~url()
        noexcept
    {
        destroy();
    }


    void
    url::create()
    {
        auto new_raw = curl_url();
        if (!new_raw)
            throw error{"curl_url() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    url::create(CURLU* handle)
    {
        destroy();
        acquire(handle);
    }


    void
    url::create(const url& other)
    {
        if (!other) {
            destroy();
            return;
        }

        auto new_raw = curl_url_dup(other.raw);
        if (!new_raw)
            throw error{"curl_url() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    url::destroy()
        noexcept
    {
        if (is_valid()) {
            auto old_raw = release();
            curl_url_cleanup(old_raw);
        }
    }


    std::string
    url::get_url(unsigned flags)
        const
    {
        return value_or_throw(try_get_url(flags));
    }


    std::expected<std::string, error>
    url::try_get_url(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_URL, flags);
    }


    std::string
    url::get_user(unsigned flags)
        const
    {
        return value_or_throw(try_get_user(flags));
    }


    std::expected<std::string, error>
    url::try_get_user(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_USER, flags);
    }


    std::string
    url::get_password(unsigned flags)
        const
    {
        return value_or_throw(try_get_password(flags));
    }


    std::expected<std::string, error>
    url::try_get_password(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_PASSWORD, flags);
    }


    std::string
    url::get_options(unsigned flags)
        const
    {
        return value_or_throw(try_get_options(flags));
    }


    std::expected<std::string, error>
    url::try_get_options(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_OPTIONS, flags);
    }


    std::string
    url::get_host(unsigned flags)
        const
    {
        return value_or_throw(try_get_host(flags));
    }


    std::expected<std::string, error>
    url::try_get_host(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_HOST, flags);
    }


    std::string
    url::get_zone_id(unsigned flags)
        const
    {
        return value_or_throw(try_get_zone_id(flags));
    }


    std::expected<std::string, error>
    url::try_get_zone_id(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_ZONEID, flags);
    }


    std::string
    url::get_port(unsigned flags)
        const
    {
        return value_or_throw(try_get_port(flags));
    }


    std::expected<std::string, error>
    url::try_get_port(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_PORT, flags);
    }


    std::string
    url::get_path(unsigned flags)
        const
    {
        return value_or_throw(try_get_path(flags));
    }


    std::expected<std::string, error>
    url::try_get_path(unsigned flags)
        const noexcept
    {
        return wrap_url_get(raw, CURLUPART_PATH, flags);
    }


    std::optional<std::string>
    url::get_query(unsigned flags)
        const
    {
        return value_or_throw(try_get_query(flags));
    }


    std::expected<std::optional<std::string>, error>
    url::try_get_query(unsigned flags)
        const noexcept
    {
        return wrap_url_get_opt(raw, CURLUPART_QUERY, flags);
    }


    std::optional<std::string>
    url::get_fragment(unsigned flags)
        const
    {
        return value_or_throw(try_get_fragment(flags));
    }


    std::expected<std::optional<std::string>, error>
    url::try_get_fragment(unsigned flags)
        const noexcept
    {
        return wrap_url_get_opt(raw, CURLUPART_FRAGMENT, flags);
    }


    void
    url::set_url(const std::string& url_arg,
                 unsigned flags)
    {
        return value_or_throw(try_set_url(url_arg, flags));
    }


    std::expected<void, error>
    url::try_set_url(const std::string url_arg,
                     unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_URL, url_arg, flags);
    }


    void
    url::unset_url(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_URL, flags);
    }


    void
    url::set_scheme(const std::string& scheme,
                    unsigned flags)
    {
        return value_or_throw(try_set_scheme(scheme, flags));
    }


    std::expected<void, error>
    url::try_set_scheme(const std::string& scheme,
                        unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_SCHEME, scheme, flags);
    }


    void
    url::unset_scheme(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_SCHEME, flags);
    }


    void
    url::set_user(const std::string& user,
                  unsigned flags)
    {
        return value_or_throw(try_set_user(user, flags));
    }


    std::expected<void, error>
    url::try_set_user(const std::string& user,
                      unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_USER, user, flags);
    }


    void
    url::unset_user(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_USER, flags);
    }


    void
    url::set_password(const std::string& password,
                      unsigned flags)
    {
        return value_or_throw(try_set_password(password, flags));
    }


    std::expected<void, error>
    url::try_set_password(const std::string& password,
                          unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_PASSWORD, password, flags);
    }


    void
    url::unset_password(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_PASSWORD, flags);
    }


    void
    url::set_options(const std::string& options,
                     unsigned flags)
    {
        return value_or_throw(try_set_options(options, flags));
    }


    std::expected<void, error>
    url::try_set_options(const std::string& options,
                         unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_OPTIONS, options, flags);
    }


    void
    url::unset_options(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_OPTIONS, flags);
    }


    void
    url::set_host(const std::string& host,
                  unsigned flags)
    {
        return value_or_throw(try_set_host(host, flags));
    }


    std::expected<void, error>
    url::try_set_host(const std::string& host,
                      unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_HOST, host, flags);
    }


    void
    url::unset_host(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_HOST, flags);
    }


    void
    url::set_zone_id(const std::string& zone_id,
                     unsigned flags)
    {
        return value_or_throw(try_set_zone_id(zone_id, flags));
    }


    std::expected<void, error>
    url::try_set_zone_id(const std::string& zone_id,
                         unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_ZONEID, zone_id, flags);
    }


    void
    url::unset_zone_id(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_ZONEID, flags);
    }


    void
    url::set_port(const std::string& port,
                  unsigned flags)
    {
        return value_or_throw(try_set_port(port, flags));
    }


    std::expected<void, error>
    url::try_set_port(const std::string& port,
                      unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_PORT, port, flags);
    }


    void
    url::set_port(std::uint16_t port,
                  unsigned flags)
    {
        return value_or_throw(try_set_port(port, flags));
    }


    std::expected<void, error>
    url::try_set_port(std::uint16_t port,
                      unsigned flags)
        noexcept
    {
        try {
            return try_set_port(std::to_string(port), flags);
        }
        catch (std::exception& e){
            return std::unexpected{error{e.what()}};
        }
    }


    void
    url::unset_port(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_PORT, flags);
    }


    void
    url::set_path(const std::string& path,
                  unsigned flags)
    {
        return value_or_throw(try_set_path(path, flags));
    }


    std::expected<void, error>
    url::try_set_path(const std::string& path,
                      unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_PATH, path, flags);
    }


    void
    url::unset_path(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_PATH, flags);
    }


    void
    url::set_query(const std::string& query,
                   unsigned flags)
    {
        return value_or_throw(try_set_query(query, flags));
    }


    std::expected<void, error>
    url::try_set_query(const std::string& query,
                       unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_QUERY, query, flags);
    }


    void
    url::unset_query(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_QUERY, flags);
    }


    void
    url::set_fragment(const std::string& fragment,
                      unsigned flags)
    {
        return value_or_throw(try_set_fragment(fragment, flags));
    }


    std::expected<void, error>
    url::try_set_fragment(const std::string& fragment,
                          unsigned flags)
        noexcept
    {
        return wrap_url_set(raw, CURLUPART_FRAGMENT, fragment, flags);
    }


    void
    url::unset_fragment(unsigned flags)
        noexcept
    {
        return wrap_url_unset(raw, CURLUPART_FRAGMENT, flags);
    }

} // namespace curl
