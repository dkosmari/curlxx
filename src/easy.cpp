/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <iostream>
#include <utility>

#include "curlxx/easy.hpp"


using std::cout;
using std::endl;


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


    template<typename T>
    std::expected<void, error>
    easy::try_setopt(CURLoption opt, T&& arg)
        noexcept
    {
        auto e = curl_easy_setopt(raw, opt, arg);
        if (e != CURLE_OK)
            return std::unexpected{error{e}};
        return {};
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


    std::expected<void, error>
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
            return std::unexpected{error{e}};
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


    std::expected<void, error>
    easy::try_perform()
        noexcept
    {
        auto e = curl_easy_perform(raw);
        if (e != CURLE_OK)
            return std::unexpected{error{e}};
        return {};
    }


    void
    easy::set_abstract_unix_socket(const std::filesystem::path& socket_path)
    {
        auto result = try_set_abstract_unix_socket(socket_path);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_abstract_unix_socket(const std::filesystem::path& socket_path)
        noexcept
    {
        return try_setopt(CURLOPT_ABSTRACT_UNIX_SOCKET, socket_path.c_str());
    }


    void
    easy::unset_abstract_unix_socket()
        noexcept
    {
        try_setopt(CURLOPT_ABSTRACT_UNIX_SOCKET, nullptr);
    }


    void
    easy::set_accept_timeout(std::chrono::milliseconds timeout)
    {
        auto result = try_set_accept_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_accept_timeout(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_ACCEPTTIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_accept_encoding(const std::string& enc)
    {
        auto result = try_set_accept_encoding(enc);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_accept_encoding(const std::string& enc)
        noexcept
    {
        return try_setopt(CURLOPT_ACCEPT_ENCODING, enc.data());
    }


    void
    easy::unset_accept_encoding()
        noexcept
    {
        try_setopt(CURLOPT_ACCEPT_ENCODING, nullptr);
    }


    void
    easy::set_address_scope(long scope_id)
    {
        auto result = try_set_address_scope(scope_id);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_address_scope(long scope_id)
        noexcept
    {
        return try_setopt(CURLOPT_ADDRESS_SCOPE, scope_id);
    }


    void
    easy::set_alt_svc(const std::filesystem::path& cache_file)
    {
        auto result = try_set_alt_svc(cache_file);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_alt_svc(const std::filesystem::path& cache_file)
        noexcept
    {
        return try_setopt(CURLOPT_ALTSVC, cache_file.c_str());
    }


    void
    easy::unset_alt_svc()
        noexcept
    {
        try_setopt(CURLOPT_ALTSVC, nullptr);
    }


    void
    easy::set_alt_svc_ctrl(long mask)
    {
        auto result = try_set_alt_svc_ctrl(mask);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_alt_svc_ctrl(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_ALTSVC_CTRL, mask);
    }


    void
    easy::set_append(bool enable)
    {
        auto result = try_set_append(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_append(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_APPEND, long{enable});
    }


    void
    easy::set_auto_referer(bool enable)
    {
        auto result = try_set_auto_referer(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_auto_referer(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_AUTOREFERER, long{enable});
    }


    void
    easy::set_aws_sig_v4(const std::string& arg)
    {
        auto result = try_set_aws_sig_v4(arg);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_aws_sig_v4(const std::string& arg)
        noexcept
    {
        return try_setopt(CURLOPT_AWS_SIGV4, arg.data());
    }


    void
    easy::unset_aws_sig_v4()
        noexcept
    {
        try_setopt(CURLOPT_AWS_SIGV4, nullptr);
    }


    void
    easy::set_buffer_size(long size)
    {
        auto result = try_set_buffer_size(size);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_buffer_size(long size)
        noexcept
    {
        return try_setopt(CURLOPT_BUFFERSIZE, size);
    }


    void
    easy::set_ca_info(const std::filesystem::path& bundle_file)
    {
        auto result = try_set_ca_info(bundle_file);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ca_info(const std::filesystem::path& bundle_file)
        noexcept
    {
        return try_setopt(CURLOPT_CAINFO, bundle_file.c_str());
    }


    void
    easy::unset_ca_info()
        noexcept
    {
        try_setopt(CURLOPT_CAINFO, nullptr);
    }


    void
    easy::set_ca_info_blob(curl_blob* bundle)
    {
        auto result = try_set_ca_info_blob(bundle);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ca_info_blob(curl_blob* bundle)
        noexcept
    {
        return try_setopt(CURLOPT_CAINFO_BLOB, bundle);
    }


    void
    easy::set_ca_path(const std::filesystem::path& bundle_dir)
    {
        auto result = try_set_ca_path(bundle_dir);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ca_path(const std::filesystem::path& bundle_dir)
        noexcept
    {
        return try_setopt(CURLOPT_CAPATH, bundle_dir.c_str());
    }


    void
    easy::unset_ca_path()
        noexcept
    {
        try_setopt(CURLOPT_CAPATH, nullptr);
    }


    void
    easy::set_ca_cache_timeout(std::chrono::seconds timeout)
    {
        auto result = try_set_ca_cache_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ca_cache_timeout(std::chrono::seconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_CA_CACHE_TIMEOUT, long(timeout.count()));
    }


    void
    easy::unset_cache_timeout()
        noexcept
    {
        try_setopt(CURLOPT_CA_CACHE_TIMEOUT, long{-1});
    }


    void
    easy::set_cert_info(bool enable)
    {
        auto result = try_set_cert_info(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cert_info(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_CERTINFO, long{enable});
    }


    void
    easy::set_connect_timeout(std::chrono::seconds timeout)
    {
        auto result = try_set_connect_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_connect_timeout(std::chrono::seconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_CONNECTTIMEOUT, long(timeout.count()));
    }


    void
    easy::set_connect_timeout(std::chrono::milliseconds timeout)
    {
        auto result = try_set_connect_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_connect_timeout(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_CONNECTTIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_connect_only(connect_only opt)
    {
        auto result = try_set_connect_only(opt);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_connect_only(connect_only opt)
        noexcept
    {
        return try_setopt(CURLOPT_CONNECT_ONLY, opt);
    }


    void
    easy::set_connect_to(const std::vector<std::string>& hosts)
    {
        auto result = try_set_connect_to(hosts);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_connect_to(const std::vector<std::string>& hosts)
        noexcept
    {
        try {
            state.connect_to_list.destroy();
            for (auto& host : hosts)
                state.connect_to_list.append(host);
            return try_setopt(CURLOPT_CONNECT_TO, state.connect_to_list.data());
        }
        catch (std::bad_alloc&) {
            return std::unexpected{error{CURLE_OUT_OF_MEMORY}};
        }
        catch (...) {
            return std::unexpected{error{CURLE_OK}};
        }
    }


    void
    easy::set_cookie(const std::string& cookie)
    {
        auto result = try_set_cookie(cookie);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cookie(const std::string& cookie)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIE, cookie.data());
    }


    void
    easy::unset_cookie()
        noexcept
    {
        try_setopt(CURLOPT_COOKIE, nullptr);
    }


    void
    easy::set_cookie_file(const std::filesystem::path& cookie_file)
    {
        auto result = try_set_cookie_file(cookie_file);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cookie_file(const std::filesystem::path& cookie_file)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIEFILE, cookie_file.c_str());
    }


    void
    easy::unset_cookie_file()
        noexcept
    {
        try_setopt(CURLOPT_COOKIEFILE, nullptr);
    }


    void
    easy::set_cookie_jar(const std::filesystem::path& jar_file)
    {
        auto result = try_set_cookie_jar(jar_file);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cookie_jar(const std::filesystem::path& jar_file)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIEJAR, jar_file.c_str());
    }


    void
    easy::unset_cookie_jar()
        noexcept
    {
        try_setopt(CURLOPT_COOKIEJAR, nullptr);
    }


    void
    easy::set_cookie_list(const std::string& cookies)
    {
        auto result = try_set_cookie_list(cookies);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cookie_list(const std::string& cookies)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIELIST, cookies.data());
    }


    void
    easy::unset_cookie_list()
        noexcept
    {
        try_setopt(CURLOPT_COOKIELIST, nullptr);
    }


    void
    easy::set_cookie_session(bool start_anew)
    {
        auto result = try_set_cookie_session(start_anew);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_cookie_session(bool start_anew)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIESESSION, long{start_anew});
    }


    void
    easy::set_copy_post_fields(const std::string& fields)
    {
        auto result = try_set_copy_post_fields(fields);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_copy_post_fields(const std::string& fields)
        noexcept
    {
        try_setopt(CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(fields.size()));
        return try_setopt(CURLOPT_COPYPOSTFIELDS, fields.data());
    }


    void
    easy::unset_copy_post_fields()
        noexcept
    {
        try_setopt(CURLOPT_COPYPOSTFIELDS, nullptr);
    }


    void
    easy::set_crlf(bool convert)
    {
        auto result = try_set_crlf(convert);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_crlf(bool convert)
        noexcept
    {
        return try_setopt(CURLOPT_CRLF, long{convert});
    }


    void
    easy::set_crl_file(const std::filesystem::path& crl_file)
    {
        auto result = try_set_crl_file(crl_file);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_crl_file(const std::filesystem::path& crl_file)
        noexcept
    {
        return try_setopt(CURLOPT_CRLFILE, crl_file.c_str());
    }


    void
    easy::unset_crl_file()
        noexcept
    {
        try_setopt(CURLOPT_CRLFILE, nullptr);
    }


    void
    easy::set_custom_request(const std::string& method)
    {
        auto result = try_set_custom_request(method);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_custom_request(const std::string& method)
        noexcept
    {
        return try_setopt(CURLOPT_CUSTOMREQUEST, method.data());
    }


    void
    easy::unset_custom_request()
        noexcept
    {
        try_setopt(CURLOPT_CUSTOMREQUEST, nullptr);
    }


    void
    easy::set_default_protocol(const std::string& protocol)
    {
        auto result = try_set_default_protocol(protocol);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_default_protocol(const std::string& protocol)
        noexcept
    {
        return try_setopt(CURLOPT_DEFAULT_PROTOCOL, protocol.data());
    }


    void
    easy::unset_default_protocol()
        noexcept
    {
        try_setopt(CURLOPT_DEFAULT_PROTOCOL, nullptr);
    }


    void
    easy::set_disallow_username_in_url(bool disallow)
    {
        auto result = try_set_disallow_username_in_url(disallow);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_disallow_username_in_url(bool disallow)
        noexcept
    {
        return try_setopt(CURLOPT_DISALLOW_USERNAME_IN_URL, long{disallow});
    }


    void
    easy::set_dns_cache_timeout(std::chrono::seconds timeout)
    {
        auto result = try_set_dns_cache_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_dns_cache_timeout(std::chrono::seconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_DNS_CACHE_TIMEOUT, long(timeout.count()));
    }


    void
    easy::unset_dns_cache_timeout()
        noexcept
    {
        try_setopt(CURLOPT_DNS_CACHE_TIMEOUT, long(-1));
    }


    void
    easy::set_file_time(bool enable)
    {
        auto result = try_set_file_time(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_file_time(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_FILETIME, long{enable});
    }


    void
    easy::set_follow_location(bool enable)
    {
        auto result = try_set_follow_location(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_follow_location(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_FOLLOWLOCATION, long{enable});
    }


    void
    easy::set_forbid_reuse(bool forbid)
    {
        auto result = try_set_forbid_reuse(forbid);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_forbid_reuse(bool forbid)
        noexcept
    {
        return try_setopt(CURLOPT_FORBID_REUSE, long{forbid});
    }


    void
    easy::set_fresh_connect(bool enable)
    {
        auto result = try_set_fresh_connect(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_fresh_connect(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_FRESH_CONNECT, long{enable});
    }


    void
    easy::set_happy_eyeballs_timeout(std::chrono::milliseconds timeout)
    {
        auto result = try_set_happy_eyeballs_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_happy_eyeballs_timeout(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_header_opt(long mask)
    {
        auto result = try_set_header_opt(mask);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_header_opt(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_HEADEROPT, mask);
    }


    void
    easy::set_http_auth(long mask)
    {
        auto result = try_set_http_auth(mask);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_http_auth(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_HTTPAUTH, mask);
    }


    void
    easy::set_http_headers(const std::vector<std::string>& headers)
    {
        auto result = try_set_http_headers(headers);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_http_headers(const std::vector<std::string>& headers)
        noexcept
    {
        try {
            state.header_list.destroy();
            for (auto& h : headers)
                state.header_list.append(h);
            return try_setopt(CURLOPT_HTTPHEADER, state.header_list.data());
        }
        catch (std::bad_alloc&) {
            return std::unexpected{error{CURLE_OUT_OF_MEMORY}};
        }
        catch (...) {
            return std::unexpected{error{CURLE_OK}};
        }
    }


    void
    easy::set_http_get(bool use_get)
    {
        auto result = try_set_http_get(use_get);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_http_get(bool use_get)
        noexcept
    {
        return try_setopt(CURLOPT_HTTPGET, long{use_get});
    }


    void
    easy::set_http_version(http_version ver)
    {
        auto result = try_set_http_version(ver);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_http_version(http_version ver)
        noexcept
    {
        return try_setopt(CURLOPT_HTTP_VERSION, ver);
    }


    void
    easy::set_input_file_size(curl_off_t size)
    {
        auto result = try_set_input_file_size(size);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_input_file_size(curl_off_t size)
        noexcept
    {
        return try_setopt(CURLOPT_INFILESIZE_LARGE, size);
    }


    void
    easy::unset_input_file_size()
        noexcept
    {
        try_setopt(CURLOPT_INFILESIZE_LARGE, curl_off_t(-1));
    }


    void
    easy::set_low_speed_limit(long limit)
    {
        auto result = try_set_low_speed_limit(limit);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_low_speed_limit(long limit)
        noexcept
    {
        return try_setopt(CURLOPT_LOW_SPEED_LIMIT, limit);
    }


    void
    easy::set_low_speed_time(std::chrono::seconds time)
    {
        auto result = try_set_low_speed_time(time);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_low_speed_time(std::chrono::seconds time)
        noexcept
    {
        return try_setopt(CURLOPT_LOW_SPEED_TIME, long(time.count()));
    }


    void
    easy::set_max_age_conn(std::chrono::seconds age)
    {
        auto result = try_set_max_age_conn(age);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_max_age_conn(std::chrono::seconds age)
        noexcept
    {
        return try_setopt(CURLOPT_MAXAGE_CONN, long(age.count()));
    }


    void
    easy::set_max_connects(long amount)
    {
        auto result = try_set_max_connects(amount);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_max_connects(long amount)
        noexcept
    {
        return try_setopt(CURLOPT_MAXCONNECTS, amount);
    }


    void
    easy::set_max_file_size(curl_off_t size)
    {
        auto result = try_set_max_file_size(size);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_max_file_size(curl_off_t size)
        noexcept
    {
        return try_setopt(CURLOPT_MAXFILESIZE_LARGE, size);
    }


    void
    easy::set_read_function(std::function<read_function_t> fn)
    {
        auto result = try_set_read_function(std::move(fn));
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_read_function(std::function<read_function_t> fn)
        noexcept
    {
        auto data_res = try_setopt(CURLOPT_READDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = try_setopt(CURLOPT_READFUNCTION, &dispatch_read_callback);
        if (!func_res)
            return func_res;

        state.read_callback = std::move(fn);
        return {};
    }


    void
    easy::set_ssl_verify_host(bool enable)
    {
        auto result = try_set_ssl_verify_host(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ssl_verify_host(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_SSL_VERIFYHOST, long{enable});
    }


    void
    easy::set_ssl_verify_peer(bool enable)
    {
        auto result = try_set_ssl_verify_peer(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ssl_verify_peer(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_SSL_VERIFYPEER, long{enable});
    }


    void
    easy::set_ssl_verify_status(bool enable)
    {
        auto result = try_set_ssl_verify_status(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ssl_verify_status(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_SSL_VERIFYSTATUS, long{enable});
    }


    void
    easy::set_tcp_keep_alive(bool enable)
    {
        auto result = try_set_tcp_keep_alive(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_tcp_keep_alive(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_TCP_KEEPALIVE, long{enable});
    }


#if CURL_AT_LEAST_VERSION(8, 9, 0)

    void
    easy::set_tcp_keep_cnt(long count)
    {
        auto result = try_set_tcp_keep_cnt(count);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_tcp_keep_cnt(long count)
        noexcept
    {
        return try_setopt(CURLOPT_TCP_KEEPCNT, count);
    }

#endif


    void
    easy::set_tcp_keep_idle(std::chrono::seconds delay)
    {
        auto result = try_set_tcp_keep_idle(delay);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_tcp_keep_idle(std::chrono::seconds delay)
        noexcept
    {
        return try_setopt(CURLOPT_TCP_KEEPIDLE, long(delay.count()));
    }


    void
    easy::set_tcp_keep_intvl(std::chrono::seconds interval)
    {
        auto result = try_set_tcp_keep_intvl(interval);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_tcp_keep_intvl(std::chrono::seconds interval)
        noexcept
    {
        return try_setopt(CURLOPT_TCP_KEEPINTVL, long(interval.count()));
    }


    void
    easy::set_tcp_no_delay(bool no_delay)
    {
        auto result = try_set_tcp_no_delay(no_delay);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_tcp_no_delay(bool no_delay)
        noexcept
    {
        return try_setopt(CURLOPT_TCP_NODELAY, long{no_delay});
    }


    void
    easy::set_timeout(std::chrono::seconds timeout)
    {
        auto result = try_set_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_timeout(std::chrono::seconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_TIMEOUT, long(timeout.count()));
    }


    void
    easy::set_timeout(std::chrono::milliseconds timeout)
    {
        auto result = try_set_timeout(timeout);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_timeout(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_TIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_transfer_text(bool enable)
    {
        auto result = try_set_transfer_text(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_transfer_text(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_TRANSFERTEXT, long{enable});
    }


    void
    easy::set_transfer_encoding(bool enable)
    {
        auto result = try_set_transfer_encoding(enable);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_transfer_encoding(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_TRANSFER_ENCODING, long{enable});
    }


    void
    easy::set_url(const std::string& url)
    {
        auto result = try_set_url(url);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_url(const std::string& url)
        noexcept
    {
        return try_setopt(CURLOPT_URL, url.data());
    }


    void
    easy::unset_url()
        noexcept
    {
        try_setopt(CURLOPT_URL, nullptr);

    }


    void
    easy::set_user_agent(const std::string& ua)
    {
        auto result = try_set_user_agent(ua);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_user_agent(const std::string& ua)
        noexcept
    {
        return try_setopt(CURLOPT_USERAGENT, ua.data());
    }


    void
    easy::unset_user_agent()
        noexcept
    {
        try_setopt(CURLOPT_USERAGENT, nullptr);
    }


    void
    easy::set_use_ssl(ssl_level level)
    {
        auto result = try_set_use_ssl(level);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_use_ssl(ssl_level level)
        noexcept
    {
        return try_setopt(CURLOPT_USE_SSL, level);
    }


    void
    easy::set_verbose(bool v)
        noexcept
    {
        try_setopt(CURLOPT_VERBOSE, long{v});
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
        auto data_res = try_setopt(CURLOPT_WRITEDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = try_setopt(CURLOPT_WRITEFUNCTION, &dispatch_write_callback);
        if (!func_res)
            return func_res;

        state.write_callback = std::move(fn);
        return {};
    }


    void
    easy::set_ws_options(long mask)
    {
        auto result = try_set_ws_options(mask);
        if (!result)
            throw result.error();
    }


    std::expected<void, error>
    easy::try_set_ws_options(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_WS_OPTIONS, mask);
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
            return std::unexpected{error{e}};
        if (!h)
            return std::unexpected{error{"no header found!"}};
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
