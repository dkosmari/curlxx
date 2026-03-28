/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <utility>

#include "curlxx/easy.hpp"

#include "utils.hpp"


using curl::utils::value_or_throw;


namespace curl {

    easy::easy()
    {
        create();
    }


    easy::easy(CURL* handle)
    {
        create(handle);
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
    easy::create(CURL* handle)
    {
        destroy();
        acquire(handle);
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
        extra_state = std::move(get<1>(new_state));
        setup_extra_state();
    }


    void
    easy::acquire(raw_type new_raw)
    {
        base_type::acquire(new_raw);
        setup_extra_state();
    }


    easy::state_type
    easy::release()
        noexcept
    {
        state_type result{
            base_type::release(),
            std::move(extra_state)
        };
        extra_state = {};
        return result;
    }


    void
    easy::reset()
    {
        if (raw) {
            curl_easy_reset(raw);
            extra_state = {};
            setup_extra_state();
        }
    }


    void
    easy::pause(bool pause_recv,
                bool pause_send)
    {
        return value_or_throw(try_pause(pause_recv, pause_send));
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
        return value_or_throw(try_perform());
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


    std::size_t
    easy::recv(void* buffer,
               std::size_t size)
    {
        return value_or_throw(try_recv(buffer, size));
    }


    std::expected<std::size_t, error>
    easy::try_recv(void* buffer,
                   std::size_t size)
        noexcept
    {
        std::size_t received;
        auto e = curl_easy_recv(raw, buffer, size, &received);
        if (e)
            return std::unexpected{error{e}};
        return received;
    }


    std::size_t
    easy::send(const void* buffer,
               std::size_t size)
    {
        return value_or_throw(try_send(buffer, size));
    }


    std::expected<std::size_t, error>
    easy::try_send(const void* buffer,
                   std::size_t size)
        noexcept
    {
        std::size_t sent;
        auto e = curl_easy_send(raw, buffer, size, &sent);
        if (e)
            return std::unexpected{error{e}};
        return sent;
    }


    void
    easy::set_abstract_unix_socket(const std::filesystem::path& socket_path)
    {
        return value_or_throw(try_set_abstract_unix_socket(socket_path));
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
        return value_or_throw(try_set_accept_timeout(timeout));
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
        return value_or_throw(try_set_accept_encoding(enc));
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
        return value_or_throw(try_set_address_scope(scope_id));
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
        return value_or_throw(try_set_alt_svc(cache_file));
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
        return value_or_throw(try_set_alt_svc_ctrl(mask));
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
        return value_or_throw(try_set_append(enable));
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
        return value_or_throw(try_set_auto_referer(enable));
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
        return value_or_throw(try_set_aws_sig_v4(arg));
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
        return value_or_throw(try_set_buffer_size(size));
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
        return value_or_throw(try_set_ca_info(bundle_file));
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
        return value_or_throw(try_set_ca_info_blob(bundle));
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
        return value_or_throw(try_set_ca_path(bundle_dir));
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
        return value_or_throw(try_set_ca_cache_timeout(timeout));
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
        return value_or_throw(try_set_cert_info(enable));
    }


    std::expected<void, error>
    easy::try_set_cert_info(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_CERTINFO, long{enable});
    }


    void
    easy::set_closesocket_function(closesocket_function_t closesocket_func)
    {
        return value_or_throw(try_set_closesocket_function(std::move(closesocket_func)));
    }


    std::expected<void, error>
    easy::try_set_closesocket_function(closesocket_function_t closesocket_func)
        noexcept
    {
        if (!closesocket_func) {
            unset_closesocket_function();
            return {};
        }

        auto data_status = try_setopt(CURLOPT_CLOSESOCKETDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = try_setopt(CURLOPT_CLOSESOCKETFUNCTION, &closesocket_function_helper);
        if (!func_status)
            return func_status;
        extra_state.closesocket_func = std::move(closesocket_func);
        return {};
    }


    void
    easy::unset_closesocket_function()
        noexcept
    {
        extra_state.closesocket_func = {};
        try_setopt(CURLOPT_CLOSESOCKETDATA, nullptr);
        try_setopt(CURLOPT_CLOSESOCKETFUNCTION, nullptr);
    }


    void
    easy::set_connect_timeout(std::chrono::seconds timeout)
    {
        return value_or_throw(try_set_connect_timeout(timeout));
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
        return value_or_throw(try_set_connect_timeout(timeout));
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
        return value_or_throw(try_set_connect_only(opt));
    }


    std::expected<void, error>
    easy::try_set_connect_only(connect_only opt)
        noexcept
    {
        return try_setopt(CURLOPT_CONNECT_ONLY, opt);
    }


    void
    easy::set_connect_to(slist hosts)
    {
        return value_or_throw(try_set_connect_to(std::move(hosts)));
    }


    std::expected<void, error>
    easy::try_set_connect_to(slist hosts)
        noexcept
    {
        auto result = try_setopt(CURLOPT_CONNECT_TO, hosts.data());
        if (result)
            extra_state.connect_to_list = std::move(hosts);
        return result;
    }


    void
    easy::set_cookie(const std::string& cookie)
    {
        return value_or_throw(try_set_cookie(cookie));
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
        return value_or_throw(try_set_cookie_file(cookie_file));
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
        return value_or_throw(try_set_cookie_jar(jar_file));
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
        return value_or_throw(try_set_cookie_list(cookies));
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
        return value_or_throw(try_set_cookie_session(start_anew));
    }


    std::expected<void, error>
    easy::try_set_cookie_session(bool start_anew)
        noexcept
    {
        return try_setopt(CURLOPT_COOKIESESSION, long{start_anew});
    }


    void
    easy::set_copy_post_fields(const std::string& data)
    {
        set_copy_post_fields(data.data(), data.size());
    }


    void
    easy::set_copy_post_fields(const void* data,
                               std::size_t size)
    {
        return value_or_throw(try_set_copy_post_fields(data, size));
    }


    std::expected<void, error>
    easy::try_set_copy_post_fields(const std::string& data)
        noexcept
    {
        return try_set_copy_post_fields(data.data(), data.size());
    }


    std::expected<void, error>
    easy::try_set_copy_post_fields(const void* data,
                                   std::size_t size)
        noexcept
    {
        auto res = try_set_post_field_size(size);
        if (!res)
            return res;
        return try_setopt(CURLOPT_COPYPOSTFIELDS, data);
    }


    void
    easy::unset_copy_post_fields()
        noexcept
    {
        try_setopt(CURLOPT_COPYPOSTFIELDS, nullptr);
    }


    void
    easy::set_private(std::any private_data)
        noexcept
    {
        extra_state.private_data = std::move(private_data);
    }


    void
    easy::set_crlf(bool convert)
    {
        return value_or_throw(try_set_crlf(convert));
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
        return value_or_throw(try_set_crl_file(crl_file));
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
        return value_or_throw(try_set_custom_request(method));
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
        return value_or_throw(try_set_default_protocol(protocol));
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
        return value_or_throw(try_set_disallow_username_in_url(disallow));
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
        return value_or_throw(try_set_dns_cache_timeout(timeout));
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
    easy::set_expect_100_timeout(std::chrono::milliseconds timeout)
    {
        return value_or_throw(try_set_expect_100_timeout(timeout));
    }


    std::expected<void, error>
    easy::try_set_expect_100_timeout(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_setopt(CURLOPT_EXPECT_100_TIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_fail_on_error(bool enable)
    {
        return value_or_throw(try_set_fail_on_error(enable));
    }


    std::expected<void, error>
    easy::try_set_fail_on_error(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_FAILONERROR, long{enable});
    }


    void
    easy::set_file_time(bool enable)
    {
        return value_or_throw(try_set_file_time(enable));
    }


    std::expected<void, error>
    easy::try_set_file_time(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_FILETIME, long{enable});
    }


    void
    easy::set_fnmatch_function(fnmatch_function_t fnmatch_func)
    {
        return value_or_throw(try_set_fnmatch_function(std::move(fnmatch_func)));
    }


    std::expected<void, error>
    easy::try_set_fnmatch_function(fnmatch_function_t fnmatch_func)
        noexcept
    {
        if (!fnmatch_func) {
            unset_fnmatch_function();
            return {};
        }

        auto data_status = try_setopt(CURLOPT_FNMATCH_DATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = try_setopt(CURLOPT_FNMATCH_FUNCTION, &fnmatch_function_helper);
        if (!func_status)
            return func_status;
        extra_state.fnmatch_func = std::move(fnmatch_func);
        return {};
    }


    void
    easy::unset_fnmatch_function()
        noexcept
    {
        extra_state.fnmatch_func = {};
        try_setopt(CURLOPT_FNMATCH_DATA, nullptr);
        try_setopt(CURLOPT_FNMATCH_FUNCTION, nullptr);
    }


    void
    easy::set_follow_location(bool enable)
    {
        return value_or_throw(try_set_follow_location(enable));
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
        return value_or_throw(try_set_forbid_reuse(forbid));
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
        return value_or_throw(try_set_fresh_connect(enable));
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
        return value_or_throw(try_set_happy_eyeballs_timeout(timeout));
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
        return value_or_throw(try_set_header_opt(mask));
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
        return value_or_throw(try_set_http_auth(mask));
    }


    std::expected<void, error>
    easy::try_set_http_auth(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_HTTPAUTH, mask);
    }


    void
    easy::set_http_headers(slist headers)
    {
        return value_or_throw(try_set_http_headers(std::move(headers)));
    }


    std::expected<void, error>
    easy::try_set_http_headers(slist headers)
        noexcept
    {
        auto result = try_setopt(CURLOPT_HTTPHEADER, headers.data());
        if (result)
            extra_state.headers_list = std::move(headers);
        return result;
    }


    void
    easy::append_header(const std::string& header)
    {
        return value_or_throw(try_append_header(header));
    }


    std::expected<void, error>
    easy::try_append_header(const std::string& header)
        noexcept
    {
        auto result = extra_state.headers_list.try_append(header);
        if (!result)
            return result;
        return try_setopt(CURLOPT_HTTPHEADER, extra_state.headers_list.data());
    }


    void
    easy::set_http_get(bool use_get)
    {
        return value_or_throw(try_set_http_get(use_get));
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
        return value_or_throw(try_set_http_version(ver));
    }


    std::expected<void, error>
    easy::try_set_http_version(http_version ver)
        noexcept
    {
        return try_setopt(CURLOPT_HTTP_VERSION, ver);
    }


    void
    easy::set_ignore_content_length(bool ignore)
    {
        return value_or_throw(try_set_ignore_content_length(ignore));
    }


    std::expected<void, error>
    easy::try_set_ignore_content_length(bool ignore)
        noexcept
    {
        return try_setopt(CURLOPT_IGNORE_CONTENT_LENGTH, long{ignore});
    }


    void
    easy::set_input_file_size(curl_off_t size)
    {
        return value_or_throw(try_set_input_file_size(size));
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
        return value_or_throw(try_set_low_speed_limit(limit));
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
        return value_or_throw(try_set_low_speed_time(time));
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
        return value_or_throw(try_set_max_age_conn(age));
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
        return value_or_throw(try_set_max_connects(amount));
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
        return value_or_throw(try_set_max_file_size(size));
    }


    std::expected<void, error>
    easy::try_set_max_file_size(curl_off_t size)
        noexcept
    {
        return try_setopt(CURLOPT_MAXFILESIZE_LARGE, size);
    }


    void
    easy::set_mime_post(mime& data)
    {
        return value_or_throw(try_set_mime_post(data));
    }


    std::expected<void, error>
    easy::try_set_mime_post(mime& data)
        noexcept
    {
        return try_setopt(CURLOPT_MIMEPOST, data.data());
    }


    void
    easy::unset_mime_post()
        noexcept
    {
        try_setopt(CURLOPT_MIMEPOST, nullptr);
    }


    void
    easy::set_mime_options(long options)
    {
        return value_or_throw(try_set_mime_options(options));
    }


    std::expected<void, error>
    easy::try_set_mime_options(long options)
        noexcept
    {
        return try_setopt(CURLOPT_MIME_OPTIONS, options);
    }


    void
    easy::set_netrc(CURL_NETRC_OPTION level)
    {
        return value_or_throw(try_set_netrc(level));
    }


    std::expected<void, error>
    easy::try_set_netrc(CURL_NETRC_OPTION level)
        noexcept
    {
        return try_setopt(CURLOPT_NETRC, long{level});
    }


    void
    easy::set_netrc_file(const std::filesystem::path& filename)
    {
        return value_or_throw(try_set_netrc_file(filename));
    }


    std::expected<void, error>
    easy::try_set_netrc_file(const std::filesystem::path& filename)
        noexcept
    {
        return try_setopt(CURLOPT_NETRC_FILE, filename.c_str());
    }


    void
    easy::unset_netrc_file()
        noexcept
    {
        try_setopt(CURLOPT_NETRC_FILE, nullptr);
    }


    void
    easy::set_new_directory_perms(long mode)
    {
        return value_or_throw(try_set_new_directory_perms(mode));
    }


    std::expected<void, error>
    easy::try_set_new_directory_perms(long mode)
        noexcept
    {
        return try_setopt(CURLOPT_NEW_DIRECTORY_PERMS, mode);
    }


    void
    easy::set_new_file_perms(long mode)
    {
        return value_or_throw(try_set_new_file_perms(mode));
    }


    std::expected<void, error>
    easy::try_set_new_file_perms(long mode)
        noexcept
    {
        return try_setopt(CURLOPT_NEW_FILE_PERMS, mode);
    }


    void
    easy::set_no_body(bool no_body)
    {
        return value_or_throw(try_set_no_body(no_body));
    }


    std::expected<void, error>
    easy::try_set_no_body(bool no_body)
        noexcept
    {
        return try_setopt(CURLOPT_NOBODY, long{no_body});
    }


    void
    easy::set_no_progress(bool no_progress)
    {
        return value_or_throw(try_set_no_progress(no_progress));
    }


    std::expected<void, error>
    easy::try_set_no_progress(bool no_progress)
        noexcept
    {
        return try_setopt(CURLOPT_NOPROGRESS, long{no_progress});
    }


    void
    easy::set_opensocket_function(opensocket_function_t opensocket_func)
    {
        return value_or_throw(try_set_opensocket_function(std::move(opensocket_func)));
    }


    std::expected<void, error>
    easy::try_set_opensocket_function(opensocket_function_t opensocket_func)
        noexcept
    {
        if (!opensocket_func) {
            unset_opensocket_function();
            return {};
        }

        auto data_status = try_setopt(CURLOPT_OPENSOCKETDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = try_setopt(CURLOPT_OPENSOCKETFUNCTION, &opensocket_function_helper);
        if (!func_status)
            return func_status;
        extra_state.opensocket_func = std::move(opensocket_func);
        return {};
    }


    void
    easy::unset_opensocket_function()
        noexcept
    {
        extra_state.opensocket_func = {};
        try_setopt(CURLOPT_OPENSOCKETDATA, nullptr);
        try_setopt(CURLOPT_OPENSOCKETFUNCTION, nullptr);
    }


    void
    easy::set_password(const std::string& password)
    {
        return value_or_throw(try_set_password(password));
    }


    std::expected<void, error>
    easy::try_set_password(const std::string& password)
        noexcept
    {
        return try_setopt(CURLOPT_PASSWORD, password.data());
    }


    void
    easy::unset_password()
        noexcept
    {
        try_setopt(CURLOPT_PASSWORD, nullptr);
    }


    void
    easy::set_port(std::uint16_t port)
    {
        return value_or_throw(try_set_port(port));
    }


    std::expected<void, error>
    easy::try_set_port(std::uint16_t port)
        noexcept
    {
        return try_setopt(CURLOPT_PORT, long{port});
    }


    void
    easy::set_post(bool enable)
    {
        return value_or_throw(try_set_post(enable));
    }


    std::expected<void, error>
    easy::try_set_post(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_POST, long{enable});
    }


    void
    easy::set_post_field(const std::string& data)
    {
        set_post_field(data.data(), data.size());
    }


    void
    easy::set_post_field(const void* data,
                         std::size_t size)
    {
        return value_or_throw(try_set_post_field(data, size));
    }


    std::expected<void, error>
    easy::try_set_post_field(const std::string& data)
        noexcept
    {
        return try_set_post_field(data.data(), data.size());
    }


    std::expected<void, error>
    easy::try_set_post_field(const void* data,
                              std::size_t size)
        noexcept
    {
        auto res = try_set_post_field_size(size);
        if (!res)
            return res;
        return try_setopt(CURLOPT_POSTFIELDS, data);
    }


    void
    easy::unset_post_field()
        noexcept
    {
        try_setopt(CURLOPT_POSTFIELDS, nullptr);
    }


    void
    easy::set_post_field_size(curl_off_t size)
    {
        return value_or_throw(try_set_post_field_size(size));
    }


    std::expected<void, error>
    easy::try_set_post_field_size(curl_off_t size)
        noexcept
    {
        return try_setopt(CURLOPT_POSTFIELDSIZE_LARGE, size);
    }


    void
    easy::set_read_data(void* data_ptr)
    {
        return value_or_throw(try_set_read_data(data_ptr));
    }


    std::expected<void, error>
    easy::try_set_read_data(void* data_ptr)
        noexcept
    {
        unset_read_function();
        return try_setopt(CURLOPT_READDATA, data_ptr);
    }


    void
    easy::set_read_function(read_function_t read_func)
    {
        return value_or_throw(try_set_read_function(std::move(read_func)));
    }


    std::expected<void, error>
    easy::try_set_read_function(read_function_t read_func)
        noexcept
    {
        if (!read_func) {
            unset_read_function();
            return {};
        }

        auto data_res = try_setopt(CURLOPT_READDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = try_setopt(CURLOPT_READFUNCTION, &read_function_helper);
        if (!func_res)
            return func_res;

        extra_state.read_func = std::move(read_func);
        return {};
    }


    void
    easy::unset_read_function()
        noexcept
    {
        extra_state.read_func = {};
        try_setopt(CURLOPT_READFUNCTION, nullptr);
    }


    void
    easy::set_ssl_verify_host(bool enable)
    {
        return value_or_throw(try_set_ssl_verify_host(enable));
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
        return value_or_throw(try_set_ssl_verify_peer(enable));
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
        return value_or_throw(try_set_ssl_verify_status(enable));
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
        return value_or_throw(try_set_tcp_keep_alive(enable));
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
        return value_or_throw(try_set_tcp_keep_cnt(count));
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
        return value_or_throw(try_set_tcp_keep_idle(delay));
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
        return value_or_throw(try_set_tcp_keep_intvl(interval));
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
        return value_or_throw(try_set_tcp_no_delay(no_delay));
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
        return value_or_throw(try_set_timeout(timeout));
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
        return value_or_throw(try_set_timeout(timeout));
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
        return value_or_throw(try_set_transfer_text(enable));
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
        return value_or_throw(try_set_transfer_encoding(enable));
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
        return value_or_throw(try_set_url(url));
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
    easy::set_user_agent(const std::string& user_agent)
    {
        return value_or_throw(try_set_user_agent(user_agent));
    }


    std::expected<void, error>
    easy::try_set_user_agent(const std::string& user_agent)
        noexcept
    {
        return try_setopt(CURLOPT_USERAGENT, user_agent.data());
    }


    void
    easy::unset_user_agent()
        noexcept
    {
        try_setopt(CURLOPT_USERAGENT, nullptr);
    }


    void
    easy::set_username(const std::string& username)
    {
        return value_or_throw(try_set_username(username));
    }


    std::expected<void, error>
    easy::try_set_username(const std::string& username)
        noexcept
    {
        return try_setopt(CURLOPT_USERNAME, username.data());
    }


    void
    easy::unset_username()
    noexcept
    {
        try_setopt(CURLOPT_USERNAME, nullptr);
    }


    void
    easy::set_use_ssl(ssl_level level)
    {
        return value_or_throw(try_set_use_ssl(level));
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
    easy::set_wildcard_match(bool enable)
    {
        return value_or_throw(try_set_wildcard_match(enable));
    }


    std::expected<void, error>
    easy::try_set_wildcard_match(bool enable)
        noexcept
    {
        return try_setopt(CURLOPT_WILDCARDMATCH, long{enable});
    }


    void
    easy::set_write_data(void* data_ptr)
    {
        return value_or_throw(try_set_write_data(data_ptr));
    }


    std::expected<void, error>
    easy::try_set_write_data(void* data_ptr)
        noexcept
    {
        unset_write_function();
        return try_setopt(CURLOPT_WRITEDATA, data_ptr);
    }


    void
    easy::set_write_function(write_function_t write_func)
    {
        return value_or_throw(try_set_write_function(std::move(write_func)));
    }


    std::expected<void, error>
    easy::try_set_write_function(write_function_t write_func)
        noexcept
    {
        if (!write_func) {
            unset_write_function();
            return {};
        }

        auto data_res = try_setopt(CURLOPT_WRITEDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = try_setopt(CURLOPT_WRITEFUNCTION, &write_function_helper);
        if (!func_res)
            return func_res;

        extra_state.write_func = std::move(write_func);
        return {};
    }


    void
    easy::unset_write_function()
        noexcept
    {
        extra_state.write_func = {};
        try_setopt(CURLOPT_WRITEFUNCTION, nullptr);
    }


    void
    easy::set_ws_options(long mask)
    {
        return value_or_throw(try_set_ws_options(mask));
    }


    std::expected<void, error>
    easy::try_set_ws_options(long mask)
        noexcept
    {
        return try_setopt(CURLOPT_WS_OPTIONS, mask);
    }


    void
    easy::set_xfer_info_function(progress_function_t progress_func)
    {
        return value_or_throw(try_set_xfer_info_function(std::move(progress_func)));
    }


    std::expected<void, error>
    easy::try_set_xfer_info_function(progress_function_t progress_func)
        noexcept
    {
        if (!progress_func) {
            unset_xfer_info_function();
            return {};
        }

        auto data_status = try_setopt(CURLOPT_XFERINFODATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = try_setopt(CURLOPT_XFERINFOFUNCTION, &progress_function_helper);
        if (!func_status)
            return func_status;
        extra_state.progress_func = std::move(progress_func);
        return {};
    }


    void
    easy::unset_xfer_info_function()
        noexcept
    {
        extra_state.progress_func = {};
        try_setopt(CURLOPT_XFERINFODATA, nullptr);
        try_setopt(CURLOPT_XFERINFOFUNCTION, nullptr);
    }


    curl_socket_t
    easy::get_active_socket()
        const
    {
        return value_or_throw(try_get_active_socket());
    }


    std::expected<curl_socket_t, error>
    easy::try_get_active_socket()
        const noexcept
    {
        return try_getinfo<curl_socket_t>(CURLINFO_ACTIVESOCKET);
    }


    std::chrono::microseconds
    easy::get_app_connect_time()
        const
    {
        return value_or_throw(try_get_app_connect_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_app_connect_time()
        const noexcept
    {
        return try_getinfo<curl_off_t, std::chrono::microseconds>(CURLINFO_APPCONNECT_TIME_T);
    }


    bool
    easy::get_condition_unmet()
        const
    {
        return value_or_throw(try_get_condition_unmet());
    }


    std::expected<bool, error>
    easy::try_get_condition_unmet()
        const noexcept
    {
        return try_getinfo<long, bool>(CURLINFO_CONDITION_UNMET);
    }


    std::chrono::microseconds
    easy::get_connect_time()
        const
    {
        return value_or_throw(try_get_connect_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_connect_time()
        const noexcept
    {
        return try_getinfo<curl_off_t, std::chrono::microseconds>(CURLINFO_CONNECT_TIME_T);
    }


#if CURL_AT_LEAST_VERSION(8, 2, 0)

    curl_off_t
    easy::get_conn_id()
        const
    {
        return value_or_throw(try_get_conn_id());
    }


    std::expected<curl_off_t, error>
    easy::try_get_conn_id()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_CONN_ID);
    }

#endif // CURL_AT_LEAST_VERSION(8, 2, 0)


    curl_off_t
    easy::get_content_length_download()
        const
    {
        return value_or_throw(try_get_content_length_download());
    }


    std::expected<curl_off_t, error>
    easy::try_get_content_length_download()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_CONTENT_LENGTH_DOWNLOAD_T);
    }


    curl_off_t
    easy::get_content_length_upload()
        const
    {
        return value_or_throw(try_get_content_length_upload());
    }


    std::expected<curl_off_t, error>
    easy::try_get_content_length_upload()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_CONTENT_LENGTH_UPLOAD_T);
    }


    std::string
    easy::get_content_type()
        const
    {
        return value_or_throw(try_get_content_type());
    }


    std::expected<std::string, error>
    easy::try_get_content_type()
        const noexcept
    {
        return try_getinfo_str(CURLINFO_CONTENT_TYPE);
    }


    slist
    easy::get_cookie_list()
        const
    {
        return value_or_throw(try_get_cookie_list());
    }


    std::expected<slist, error>
    easy::try_get_cookie_list()
        const noexcept
    {
        return try_getinfo<curl_slist*, slist>(CURLINFO_COOKIELIST);
    }


#if CURL_AT_LEAST_VERSION(8, 11, 0)

    curl_off_t
    easy::get_early_data_sent()
        const
    {
        return value_or_throw(try_get_early_data_sent());
    }


    std::expected<curl_off_t, error>
    easy::try_get_early_data_sent()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_EARLYDATA_SENT_T);
    }

#endif // CURL_AT_LEAST_VERSION(8, 11, 0)


    std::string
    easy::get_effective_method()
        const
    {
        return value_or_throw(try_get_effective_method());
    }


    std::expected<std::string, error>
    easy::try_get_effective_method()
        const noexcept
    {
        return try_getinfo_str(CURLINFO_EFFECTIVE_METHOD);
    }


    std::string
    easy::get_effective_url()
        const
    {
        return value_or_throw(try_get_effective_url());
    }


    std::expected<std::string, error>
    easy::try_get_effective_url()
        const noexcept
    {
        return try_getinfo_str(CURLINFO_EFFECTIVE_URL);
    }


    std::chrono::utc_seconds
    easy::get_file_time()
        const
    {
        return value_or_throw(try_get_file_time());
    }


    std::expected<std::chrono::utc_seconds, error>
    easy::try_get_file_time()
        const noexcept
    {
        auto value = try_getinfo<curl_off_t>(CURLINFO_FILETIME_T);
        if (!value)
            return std::unexpected{std::move(value.error())};
        return std::chrono::utc_seconds{std::chrono::seconds(*value)};
    }


    easy::http_version
    easy::get_http_version()
        const
    {
        return value_or_throw(try_get_http_version());
    }


    std::expected<easy::http_version, error>
    easy::try_get_http_version()
        const noexcept
    {
        return try_getinfo<long, http_version>(CURLINFO_HTTP_VERSION);
    }


    const std::any&
    easy::get_private()
        const
    {
        return extra_state.private_data;
    }


    std::any&
    easy::get_private()
    {
        return extra_state.private_data;
    }


    long
    easy::get_response_code()
        const
    {
        return value_or_throw(try_get_response_code());
    }


    std::expected<long, error>
    easy::try_get_response_code()
        const noexcept
    {
        return try_getinfo<long>(CURLINFO_RESPONSE_CODE);
    }


    std::chrono::seconds
    easy::get_retry_after()
        const
    {
        return value_or_throw(try_get_retry_after());
    }


    std::expected<std::chrono::seconds, error>
    easy::try_get_retry_after()
        const noexcept
    {
        return try_getinfo<long, std::chrono::seconds>(CURLINFO_RETRY_AFTER);
    }


    curl_off_t
    easy::get_size_download()
        const
    {
        return value_or_throw(try_get_size_download());
    }


    std::expected<curl_off_t, error>
    easy::try_get_size_download()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_SIZE_DOWNLOAD_T);
    }


    curl_off_t
    easy::get_size_upload()
        const
    {
        return value_or_throw(try_get_size_upload());
    }


    std::expected<curl_off_t, error>
    easy::try_get_size_upload()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_SIZE_UPLOAD_T);
    }


    curl_off_t
    easy::get_speed_download()
        const
    {
        return value_or_throw(try_get_speed_download());
    }


    std::expected<curl_off_t, error>
    easy::try_get_speed_download()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_SPEED_DOWNLOAD_T);
    }


    curl_off_t
    easy::get_speed_upload()
        const
    {
        return value_or_throw(try_get_speed_upload());
    }


    std::expected<curl_off_t, error>
    easy::try_get_speed_upload()
        const noexcept
    {
        return try_getinfo<curl_off_t>(CURLINFO_SPEED_UPLOAD_T);
    }


    std::chrono::microseconds
    easy::get_start_transfer_time()
        const
    {
        return value_or_throw(try_get_start_transfer_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_start_transfer_time()
        const noexcept
    {
        return try_getinfo<curl_off_t, std::chrono::microseconds>(CURLINFO_STARTTRANSFER_TIME_T);
    }


    std::chrono::microseconds
    easy::get_total_time()
        const
    {
        return value_or_throw(try_get_total_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_total_time()
        const noexcept
    {
        return try_getinfo<curl_off_t, std::chrono::microseconds>(CURLINFO_TOTAL_TIME_T);
    }


    header
    easy::get_header(const std::string& name,
                     std::size_t index,
                     unsigned origin,
                     int request)
        const
    {
        return value_or_throw(try_get_header(name, index, origin, request));
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


    void
    easy::setup_extra_state()
    {
        if (raw) {
            // Link the C++ wrapper to the C instance.
            curl_easy_setopt(raw, CURLOPT_PRIVATE, this);

            // Make sure we got space in the error buffer to report detailed error messages.
            extra_state.error_buffer.resize(CURL_ERROR_SIZE);
            extra_state.error_buffer[0] = '\0';
            curl_easy_setopt(raw,
                             CURLOPT_ERRORBUFFER,
                             extra_state.error_buffer.data());

            curl_easy_setopt(raw, CURLOPT_STDERR, stdout); // TODO: remove this
        } else {
            extra_state = {};
        }
    }


    int
    easy::closesocket_function_helper(CURL* handle,
                                      curl_socket_t fd)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return 1;

        if (!ez->extra_state.closesocket_func)
            return 1;

        try {
            return ez->extra_state.closesocket_func(fd);
        }
        catch (...) {
            return 1;
        }
    }



    int
    easy::fnmatch_function_helper(CURL* handle,
                                  const char* pattern,
                                  const char* text)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_FNMATCHFUNC_FAIL;

        if (!ez->extra_state.fnmatch_func)
            return CURL_FNMATCHFUNC_FAIL;

        try {
            if (ez->extra_state.fnmatch_func(pattern, text))
                return CURL_FNMATCHFUNC_MATCH;
            else
                return CURL_FNMATCHFUNC_NOMATCH;
        }
        catch (...) {
            return CURL_FNMATCHFUNC_FAIL;
        }
    }


    curl_socket_t
    easy::opensocket_function_helper(CURL* handle,
                                     curlsocktype purpose,
                                     curl_sockaddr* address)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_SOCKET_BAD;

        if (!ez->extra_state.opensocket_func)
            return CURL_SOCKET_BAD;

        try {
            return ez->extra_state.opensocket_func(purpose, address);
        }
        catch (...) {
            return CURL_SOCKET_BAD;
        }
    }


    int
    easy::progress_function_helper(CURL* handle,
                                   curl_off_t dltotal,
                                   curl_off_t dlnow,
                                   curl_off_t ultotal,
                                   curl_off_t ulnow)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return 1; // cause CURLE_ABORTED_BY_CALLBACK error

        if (!ez->extra_state.progress_func)
            return CURL_PROGRESSFUNC_CONTINUE; // fall back to built-in progress callback

        try {
            return ez->extra_state.progress_func(dltotal, dlnow, ultotal, ulnow);
        }
        catch (...) {
            return 1; // cause CURLE_ABORTED_BY_CALLBACK error
        }
    }


    std::size_t
    easy::read_function_helper(char* buf,
                               std::size_t,
                               std::size_t size,
                               CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_READFUNC_ABORT;

        if (!ez->extra_state.read_func)
            return CURL_READFUNC_ABORT;

        try {
            return ez->extra_state.read_func({buf, size});
        }
        catch (...) {
            return CURL_READFUNC_ABORT;
        }
    }


    std::size_t
    easy::write_function_helper(const char* buffer,
                                std::size_t,
                                std::size_t size,
                                CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez)
            return CURL_WRITEFUNC_ERROR;

        if (!ez->extra_state.write_func)
            return CURL_WRITEFUNC_ERROR;

        try {
            return ez->extra_state.write_func({buffer, size});
        }
        catch (...) {
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


    template<typename T,
             typename U>
    std::expected<U, error>
    easy::try_getinfo(CURLINFO info)
        const noexcept
    {
        T result;
        auto e = curl_easy_getinfo(raw, info, &result);
        if (e)
            return std::unexpected{error{e}};
        return static_cast<U>(result);
    }


    // Special version that handles null pointers.
    std::expected<std::string, error>
    easy::try_getinfo_str(CURLINFO info)
        const noexcept
    {
        char* str;
        auto e = curl_easy_getinfo(raw, info, &str);
        if (e)
            return std::unexpected{error{e}};
        if (str)
            return str;
        return {};
    }

} // namespace curl
