/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <iostream>
#include <utility>

#include "curlxx/easy.hpp"


using std::cout;
using std::endl;
using std::expected;
using std::unexpected;


namespace curl {

    void
    easy::init_error_buffer()
    {
        state.error_buffer.resize(CURL_ERROR_SIZE);
        state.error_buffer[0] = '\0';
        curl_easy_setopt(raw,
                         CURLOPT_ERRORBUFFER,
                         state.error_buffer.data());
        curl_easy_setopt(raw, CURLOPT_STDERR, stdout);
    }


    void
    easy::link_this()
    {
        curl_easy_setopt(raw, CURLOPT_PRIVATE, this);
    }


    std::size_t
    easy::dispatch_read_callback(char* buf,
                                 std::size_t,
                                 std::size_t size,
                                 CURL* handle)
        noexcept
    {
        if (!handle)
            return CURL_READFUNC_ABORT;

        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_READFUNC_ABORT;

        try {
            if (!ez->state.read_callback)
                return CURL_READFUNC_ABORT;
            return ez->state.read_callback({buf, size});
        }
        catch (std::exception& e) {
            cout << "Error in easy::dispatch_read_callback(): " << e.what() << endl;
            return CURL_READFUNC_ABORT;
        }
        catch (...) {
            cout << "Unknown error in easy::dispatch_read_callback()" << endl;
            return CURL_READFUNC_ABORT;
        }
    }


    std::size_t
    easy::dispatch_write_callback(const char* buffer,
                                  std::size_t,
                                  std::size_t size,
                                  CURL* handle)
        noexcept
    {
        if (!handle)
            return CURL_WRITEFUNC_ERROR;

        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_WRITEFUNC_ERROR;

        try {
            if (!ez->state.write_callback)
                return CURL_WRITEFUNC_ERROR;
            return ez->state.write_callback({buffer, size});
        }
        catch (std::exception& e) {
            cout << "Error in easy::dispatch_write_callback(): " << e.what() << endl;
            return CURL_WRITEFUNC_ERROR;
        }
        catch (...) {
            cout << "Unknown error in easy::dispatch_write_callback()" << endl;
            return CURL_WRITEFUNC_ERROR;
        }
    }


    easy::easy()
    {
        create();
    }


    easy::easy(const easy& other)
    {
        create(other);
    }


    easy::easy(easy&& other)
        noexcept
    {
        acquire(other.release());
    }


    easy&
    easy::operator =(const easy& other)
    {
        create(other);
        return *this;
    }


    easy&
    easy::operator =(easy&& other)
        noexcept
    {
        if (this != &other) {
            destroy();
            acquire(other.release());
        }
        return *this;
    }


    easy::~easy()
        noexcept
    {
        destroy();
    }


    void
    easy::create()
    {
        auto new_raw = curl_easy_init();
        if (!new_raw)
            throw error{"curl_easy_init() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    easy::create(const easy& other)
    {
        if (!other) {
            destroy();
            return;
        }

        auto new_raw = curl_easy_duphandle(other.raw);
        if (!new_raw)
            throw error{"curl_easy_duphandle() failed"};

        destroy();
        acquire(new_raw);
    }


    void
    easy::destroy()
        noexcept
    {
        if (is_valid()) {
            auto [old_raw, old_state] = release();
            curl_easy_cleanup(old_raw);
        }
    }


    void
    easy::acquire(state_type new_state)
         noexcept
    {
        base_type::acquire(get<0>(new_state));
        state = std::move(get<1>(new_state));
        init_error_buffer();
        link_this();
    }


    void
    easy::acquire(raw_type new_raw)
    {
        base_type::acquire(new_raw);
        init_error_buffer();
        link_this();
    }


    easy::state_type
    easy::release()
        noexcept
    {
        state_type result{
            base_type::release(),
            extra_state_type{
                std::move(state.error_buffer),
                std::move(state.read_callback),
                std::move(state.write_callback)
            }
        };
        state = {};
        return result;
    }


    void
    easy::reset()
    {
        if (raw) {
            curl_easy_reset(raw);
            link_this();
            init_error_buffer();
        }
    }


    void
    easy::pause(bool pause_recv,
                bool pause_send)
    {
        auto result = try_pause(pause_recv, pause_send);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_pause(bool pause_recv,
                    bool pause_send)
        noexcept
    {
        int flags = 0;
        if (pause_recv)
            flags |= CURLPAUSE_RECV;
        if (pause_send)
            flags |= CURLPAUSE_SEND;
        auto e = curl_easy_pause(raw, flags);
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::unpause()
    {
        pause(false, false);
    }


    void
    easy::perform()
    {
        auto result = try_perform();
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_perform()
        noexcept
    {
        auto e = curl_easy_perform(raw);
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_follow(bool enable)
    {
        auto result = try_set_follow(enable);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_follow(bool enable)
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_FOLLOWLOCATION,
                                  long{enable});
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_forbid_reuse(bool forbid)
    {
        auto result = try_set_forbid_reuse(forbid);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_forbid_reuse(bool forbid)
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_FORBID_REUSE,
                                  long{forbid});
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_http_headers(const std::vector<std::string>& headers)
    {
        state.header_list.destroy();
        for (auto& h : headers)
            state.header_list.append(h);
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_HTTPHEADER,
                                  state.header_list.data());
        if (e != CURLE_OK)
            throw error{e};
    }


    expected<void, error>
    easy::try_set_http_headers(const std::vector<std::string>& headers)
        noexcept
    {
        try {
            state.header_list.destroy();
            for (auto& h : headers)
                state.header_list.append(h);
            auto e = curl_easy_setopt(raw,
                                      CURLOPT_HTTPHEADER,
                                      state.header_list.data());
            if (e != CURLE_OK)
                return unexpected{error{e}};
        }
        catch (std::bad_alloc&) {
            return unexpected{error{CURLE_OUT_OF_MEMORY}};
        }
        return {};
    }


    void
    easy::set_ssl_verify_peer(bool enabled)
    {
        auto result = try_set_ssl_verify_peer(enabled);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_ssl_verify_peer(bool enabled)
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_SSL_VERIFYPEER,
                                  long{enabled});
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_read_function(std::function<read_function_t> fn)
    {
        auto result = try_set_read_function(std::move(fn));
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_read_function(std::function<read_function_t> fn)
        noexcept
    {
        auto e1 = curl_easy_setopt(raw,
                                   CURLOPT_READFUNCTION,
                                   &dispatch_read_callback);
        if (e1 != CURLE_OK)
            return unexpected{error{e1}};
        auto e2 = curl_easy_setopt(raw, CURLOPT_READDATA, raw);
        if (e2 != CURLE_OK)
            return unexpected{error{e2}};
        state.read_callback = std::move(fn);
        return {};
    }


    void
    easy::set_url(const std::string& url)
    {
        auto result = try_set_url(url);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_url(const std::string& url)
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_URL,
                                  url.data());
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::unset_url()
    {
        auto result = try_unset_url();
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_unset_url()
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_URL,
                                  nullptr);
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_user_agent(const std::string& ua)
    {
        auto result = try_set_user_agent(ua);
        if (!result)
            throw result.error();
    }


    expected<void, error>
    easy::try_set_user_agent(const std::string& ua)
        noexcept
    {
        auto e = curl_easy_setopt(raw,
                                  CURLOPT_USERAGENT,
                                  ua.data());
        if (e != CURLE_OK)
            return unexpected{error{e}};
        return {};
    }


    void
    easy::set_verbose(bool v)
        noexcept
    {
         curl_easy_setopt(raw,
                          CURLOPT_VERBOSE,
                          long{v});
    }


    void
    easy::set_write_function(std::function<write_function_t> fn)
    {
        auto result = try_set_write_function(std::move(fn));
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_write_function(std::function<write_function_t> fn)
        noexcept
    {
        auto e1 = curl_easy_setopt(raw,
                                   CURLOPT_WRITEFUNCTION,
                                   &dispatch_write_callback);
        if (e1 != CURLE_OK)
            return unexpected{error{e1}};
        auto e2 = curl_easy_setopt(raw,
                                   CURLOPT_WRITEDATA,
                                   raw);
        if (e2 != CURLE_OK)
            return unexpected{error{e2}};
        state.write_callback = std::move(fn);
        return {};
    }


    header
    easy::get_header(const std::string& name,
                     std::size_t index,
                     unsigned origin,
                     int request)
        const
    {
        auto result = try_get_header(name, index, origin, request);
        if (!result)
            throw result.error();
        return std::move(*result);
    }


    std::expected<header, error>
    easy::try_get_header(const std::string& name,
                         std::size_t index,
                         unsigned origin,
                         int request)
        const noexcept
    {
        curl_header* h = nullptr;
        auto e = curl_easy_header(raw,
                                  name.data(),
                                  index,
                                  origin,
                                  request,
                                  &h);
        if (e != CURLHE_OK)
            return unexpected{error{e}};
        if (!h)
            return unexpected{error{"no header found!"}};
        return header{h};
    }


    easy*
    easy::get_wrapper(CURL* handle)
        noexcept
    {
        if (!handle)
            return nullptr;
        easy* result = nullptr;
        auto e = curl_easy_getinfo(handle,
                                   CURLINFO_PRIVATE,
                                   &result);
        if (e != CURLE_OK)
            return nullptr;
        return result;
    }

} // namespace curl
