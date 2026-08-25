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

    namespace {

        /*-----------------------*/
        /* Function declarations */
        /*-----------------------*/

        template<typename T>
        std::expected<void, error>
        wrap_setopt(CURL* raw,
                    CURLoption opt,
                    T&& arg)
            noexcept;

        std::expected<void, error>
        wrap_setopt(CURL* raw,
                    CURLoption opt,
                    bool arg)
            noexcept;

        std::expected<void, error>
        wrap_setopt(CURL* raw,
                   CURLoption opt,
                    const std::string& arg)
            noexcept;

        void
        wrap_unsetopt(CURL* raw,
                      CURLoption opt)
            noexcept;


        template<typename T,
                 typename U = T>
        std::expected<U, error>
        wrap_getinfo(CURL* raw,
                    CURLINFO info)
            noexcept;


        std::expected<std::string, error>
        wrap_getinfo_str(CURL* raw,
                        CURLINFO info)
            noexcept;


        /*----------------------*/
        /* Function definitions */
        /*----------------------*/

        template<typename T>
        std::expected<void, error>
        wrap_setopt(CURL* raw,
                    CURLoption opt,
                    T&& arg)
            noexcept
        {
            auto e = curl_easy_setopt(raw, opt, arg);
            if (e != CURLE_OK)
                return std::unexpected{error{e}};
            return {};
        }


        std::expected<void, error>
        wrap_setopt(CURL* raw,
                    CURLoption opt,
                    bool arg)
            noexcept
        {
            return wrap_setopt(raw, opt, long{arg});
        }


        std::expected<void, error>
        wrap_setopt(CURL* raw,
                   CURLoption opt,
                    const std::string& arg)
            noexcept
        {
            return wrap_setopt(raw, opt, arg.data());
        }


        void
        wrap_unsetopt(CURL* raw,
                      CURLoption opt)
            noexcept
        {
            curl_easy_setopt(raw, opt, static_cast<void*>(nullptr));
        }


        template<typename T,
                 typename U>
        std::expected<U, error>
        wrap_getinfo(CURL* raw,
                     CURLINFO info)
            noexcept
        {
            T result;
            auto e = curl_easy_getinfo(raw, info, &result);
            if (e)
                return std::unexpected{error{e}};
            return static_cast<U>(result);
        }


        // Special version that handles null pointers.
        std::expected<std::string, error>
        wrap_getinfo_str(CURL* raw,
                         CURLINFO info)
            noexcept
        {
            char* str;
            auto e = curl_easy_getinfo(raw, info, &str);
            if (e)
                return std::unexpected{error{e}};
            if (str)
                return str;
            return {};
        }

    } // namespace


    /*------------------*/
    /* Public functions */
    /*------------------*/

    easy::easy()
    {
        create();
    }


    easy::easy(CURL* handle)
    {
        create(handle);
    }


    easy::easy(const easy& other) :
        basic_wrapper{}
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
        return wrap_setopt(raw, CURLOPT_ABSTRACT_UNIX_SOCKET, socket_path.c_str());
    }


    void
    easy::unset_abstract_unix_socket()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_ABSTRACT_UNIX_SOCKET);
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
        return wrap_setopt(raw, CURLOPT_ACCEPTTIMEOUT_MS, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_ACCEPT_ENCODING, enc);
    }


    void
    easy::unset_accept_encoding()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_ACCEPT_ENCODING);
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
        return wrap_setopt(raw, CURLOPT_ADDRESS_SCOPE, scope_id);
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
        return wrap_setopt(raw, CURLOPT_ALTSVC, cache_file.c_str());
    }


    void
    easy::unset_alt_svc()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_ALTSVC);
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
        return wrap_setopt(raw, CURLOPT_ALTSVC_CTRL, mask);
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
        return wrap_setopt(raw, CURLOPT_APPEND, long{enable});
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
        return wrap_setopt(raw, CURLOPT_AUTOREFERER, enable);
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
        return wrap_setopt(raw, CURLOPT_AWS_SIGV4, arg);
    }


    void
    easy::unset_aws_sig_v4()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_AWS_SIGV4);
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
        return wrap_setopt(raw, CURLOPT_BUFFERSIZE, size);
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
        return wrap_setopt(raw, CURLOPT_CAINFO, bundle_file.c_str());
    }


    void
    easy::unset_ca_info()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_CAINFO);
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
        return wrap_setopt(raw, CURLOPT_CAINFO_BLOB, bundle);
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
        return wrap_setopt(raw, CURLOPT_CAPATH, bundle_dir.c_str());
    }


    void
    easy::unset_ca_path()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_CAPATH);
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
        return wrap_setopt(raw, CURLOPT_CA_CACHE_TIMEOUT, long(timeout.count()));
    }


    void
    easy::unset_cache_timeout()
        noexcept
    {
        std::ignore = wrap_setopt(raw, CURLOPT_CA_CACHE_TIMEOUT, long{-1});
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
        return wrap_setopt(raw, CURLOPT_CERTINFO, enable);
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

        auto data_status = wrap_setopt(raw, CURLOPT_CLOSESOCKETDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_CLOSESOCKETFUNCTION, &closesocket_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_CLOSESOCKETDATA);
        wrap_unsetopt(raw, CURLOPT_CLOSESOCKETFUNCTION);
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
        return wrap_setopt(raw, CURLOPT_CONNECTTIMEOUT, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_CONNECTTIMEOUT_MS, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_CONNECT_ONLY, opt);
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
        auto result = wrap_setopt(raw, CURLOPT_CONNECT_TO, hosts.data());
        if (result)
            extra_state.connect_to = std::move(hosts);
        return result;
    }


    void
    easy::unset_connect_to()
        noexcept
    {
        extra_state.connect_to.destroy();
        wrap_unsetopt(raw, CURLOPT_CONNECT_TO);
    }


    void
    easy::append_connect_to(const std::string& host)
    {
        return value_or_throw(try_append_connect_to(host));
    }


    std::expected<void, error>
    easy::try_append_connect_to(const std::string& host)
        noexcept
    {
        auto result = extra_state.connect_to.try_append(host);
        if (result)
            return result;
        return wrap_setopt(raw,
                           CURLOPT_CONNECT_TO,
                           extra_state.connect_to.data());
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
        return wrap_setopt(raw, CURLOPT_COOKIE, cookie);
    }


    void
    easy::unset_cookie()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_COOKIE);
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
        return wrap_setopt(raw, CURLOPT_COOKIEFILE, cookie_file.c_str());
    }


    void
    easy::unset_cookie_file()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_COOKIEFILE);
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
        return wrap_setopt(raw, CURLOPT_COOKIEJAR, jar_file.c_str());
    }


    void
    easy::unset_cookie_jar()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_COOKIEJAR);
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
        return wrap_setopt(raw, CURLOPT_COOKIELIST, cookies);
    }


    void
    easy::unset_cookie_list()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_COOKIELIST);
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
        return wrap_setopt(raw, CURLOPT_COOKIESESSION, start_anew);
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
        return wrap_setopt(raw, CURLOPT_COPYPOSTFIELDS, data);
    }


    void
    easy::unset_copy_post_fields()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_COPYPOSTFIELDS);
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
        return wrap_setopt(raw, CURLOPT_CRLF, convert);
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
        return wrap_setopt(raw, CURLOPT_CRLFILE, crl_file.c_str());
    }


    void
    easy::unset_crl_file()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_CRLFILE);
    }



    void
    easy::set_url(url url_obj)
    {
        value_or_throw(try_set_url(std::move(url_obj)));
    }


    std::expected<void, error>
    easy::try_set_url(url url_obj)
        noexcept
    {
        extra_state.url_obj = std::move(url_obj);
        return wrap_setopt(raw, CURLOPT_CURLU, extra_state.url_obj);
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
        return wrap_setopt(raw, CURLOPT_CUSTOMREQUEST, method);
    }


    void
    easy::unset_custom_request()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_CUSTOMREQUEST);
    }


    void
    easy::set_debug_function(debug_function_t debug_func)
    {
        return value_or_throw(try_set_debug_function(std::move(debug_func)));
    }


    std::expected<void, error>
    easy::try_set_debug_function(debug_function_t debug_func)
        noexcept
    {
        if (!debug_func) {
            unset_debug_function();
            return {};
        }

        auto data_status = wrap_setopt(raw, CURLOPT_DEBUGDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_DEBUGFUNCTION, &debug_callback_helper);
        if (!func_status)
            return func_status;
        extra_state.debug_func = std::move(debug_func);
        return {};
    }


    void
    easy::unset_debug_function()
        noexcept
    {
        extra_state.debug_func = {};
        wrap_unsetopt(raw, CURLOPT_DEBUGDATA);
        wrap_unsetopt(raw, CURLOPT_DEBUGFUNCTION);
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
        return wrap_setopt(raw, CURLOPT_DEFAULT_PROTOCOL, protocol);
    }


    void
    easy::unset_default_protocol()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_DEFAULT_PROTOCOL);
    }


    void
    easy::set_dir_list_only(bool enable)
    {
        return value_or_throw(try_set_dir_list_only(enable));
    }


    std::expected<void, error>
    easy::try_set_dir_list_only(bool enable)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_DIRLISTONLY, enable);
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
        return wrap_setopt(raw, CURLOPT_DISALLOW_USERNAME_IN_URL, disallow);
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
        return wrap_setopt(raw, CURLOPT_DNS_CACHE_TIMEOUT, long(timeout.count()));
    }


    void
    easy::unset_dns_cache_timeout()
        noexcept
    {
        std::ignore = wrap_setopt(raw, CURLOPT_DNS_CACHE_TIMEOUT, long(-1));
    }


    void
    easy::set_dns_servers(const std::string& servers)
    {
        return value_or_throw(try_set_dns_servers(servers));
    }


    std::expected<void, error>
    easy::try_set_dns_servers(const std::string& servers)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_DNS_SERVERS, servers);
    }


    void
    easy::unset_dns_servers()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_DNS_SERVERS);
    }


    void
    easy::set_dns_shuffle_addresses(bool enable)
    {
        return value_or_throw(try_set_dns_shuffle_addresses(enable));
    }


    std::expected<void, error>
    easy::try_set_dns_shuffle_addresses(bool enable)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_DNS_SHUFFLE_ADDRESSES, enable);
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
        return wrap_setopt(raw, CURLOPT_EXPECT_100_TIMEOUT_MS, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_FAILONERROR, enable);
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
        return wrap_setopt(raw, CURLOPT_FILETIME, enable);
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

        auto data_status = wrap_setopt(raw, CURLOPT_FNMATCH_DATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_FNMATCH_FUNCTION, &fnmatch_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_FNMATCH_DATA);
        wrap_unsetopt(raw, CURLOPT_FNMATCH_FUNCTION);
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
        return wrap_setopt(raw, CURLOPT_FOLLOWLOCATION, enable);
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
        return wrap_setopt(raw, CURLOPT_FORBID_REUSE, forbid);
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
        return wrap_setopt(raw, CURLOPT_FRESH_CONNECT, enable);
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
        return wrap_setopt(raw, CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, long(timeout.count()));
    }


    void
    easy::set_header(bool enable)
    {
        return value_or_throw(try_set_header(enable));
    }


    std::expected<void, error>
    easy::try_set_header(bool enable)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_HEADER, enable);
    }


    void
    easy::set_header_function(header_function_t header_func)
    {
        return value_or_throw(try_set_header_function(std::move(header_func)));
    }


    std::expected<void, error>
    easy::try_set_header_function(header_function_t header_func)
        noexcept
    {
        if (!header_func) {
            unset_header_function();
            return {};
        }

        auto data_status = wrap_setopt(raw, CURLOPT_HEADERDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_HEADERFUNCTION, &header_callback_helper);
        if (!func_status)
            return func_status;
        extra_state.header_func = std::move(header_func);
        return {};
    }


    void
    easy::unset_header_function()
        noexcept
    {
        extra_state.header_func = {};
        wrap_unsetopt(raw, CURLOPT_HEADERDATA);
        wrap_unsetopt(raw, CURLOPT_HEADERFUNCTION);
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
        return wrap_setopt(raw, CURLOPT_HEADEROPT, mask);
    }


    void
    easy::set_http_09_allowed(bool allowed)
    {
        return value_or_throw(try_set_http_09_allowed(allowed));
    }


    std::expected<void, error>
    easy::try_set_http_09_allowed(bool allowed)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_HTTP09_ALLOWED, allowed);
    }


    void
    easy::set_http_200_aliases(slist aliases)
    {
        return value_or_throw(try_set_http_200_aliases(std::move(aliases)));
    }


    std::expected<void, error>
    easy::try_set_http_200_aliases(slist aliases)
        noexcept
    {
        auto result = wrap_setopt(raw, CURLOPT_HTTP200ALIASES, aliases.data());
        if (result)
            extra_state.http_200_aliases = std::move(aliases);
        return result;
    }


    void
    easy::unset_http_200_aliases()
        noexcept
    {
        extra_state.http_200_aliases.destroy();
        wrap_unsetopt(raw, CURLOPT_HTTP200ALIASES);
    }


    void
    easy::append_http_200_aliases(const std::string& alias)
    {
        return value_or_throw(try_append_http_200_aliases(alias));
    }


    std::expected<void, error>
    easy::try_append_http_200_aliases(const std::string& alias)
        noexcept
    {
        auto result = extra_state.http_200_aliases.try_append(alias);
        if (!result)
            return result;
        return wrap_setopt(raw,
                           CURLOPT_HTTP200ALIASES,
                           extra_state.http_200_aliases.data());
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
        return wrap_setopt(raw, CURLOPT_HTTPAUTH, mask);
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
        return wrap_setopt(raw, CURLOPT_HTTPGET, use_get);
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
        auto result = wrap_setopt(raw, CURLOPT_HTTPHEADER, headers.data());
        if (result)
            extra_state.http_headers = std::move(headers);
        return result;
    }


    void
    easy::append_http_header(const std::string& header)
    {
        return value_or_throw(try_append_http_header(header));
    }


    std::expected<void, error>
    easy::try_append_http_header(const std::string& header)
        noexcept
    {
        auto result = extra_state.http_headers.try_append(header);
        if (!result)
            return result;
        return wrap_setopt(raw,
                           CURLOPT_HTTPHEADER,
                           extra_state.http_headers.data());
    }


    void
    easy::set_http_content_decoding(bool enable)
    {
        return value_or_throw(try_set_http_content_decoding(enable));
    }


    std::expected<void, error>
    easy::try_set_http_content_decoding(bool enable)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_HTTP_CONTENT_DECODING, enable);
    }


    void
    easy::set_http_transfer_decoding(bool enable)
    {
        return value_or_throw(try_set_http_transfer_decoding(enable));
    }


    std::expected<void, error>
    easy::try_set_http_transfer_decoding(bool enable)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_HTTP_TRANSFER_DECODING, enable);
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
        return wrap_setopt(raw, CURLOPT_HTTP_VERSION, ver);
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
        return wrap_setopt(raw, CURLOPT_IGNORE_CONTENT_LENGTH, ignore);
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
        return wrap_setopt(raw, CURLOPT_INFILESIZE_LARGE, size);
    }


    void
    easy::unset_input_file_size()
        noexcept
    {
        std::ignore = wrap_setopt(raw, CURLOPT_INFILESIZE_LARGE, curl_off_t(-1));
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
        return wrap_setopt(raw, CURLOPT_LOW_SPEED_LIMIT, limit);
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
        return wrap_setopt(raw, CURLOPT_LOW_SPEED_TIME, long(time.count()));
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
        return wrap_setopt(raw, CURLOPT_MAXAGE_CONN, long(age.count()));
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
        return wrap_setopt(raw, CURLOPT_MAXCONNECTS, amount);
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
        return wrap_setopt(raw, CURLOPT_MAXFILESIZE_LARGE, size);
    }


    void
    easy::set_max_lifetime_conn(std::chrono::seconds lifetime)
    {
        return value_or_throw(try_set_max_lifetime_conn(lifetime));
    }


    std::expected<void, error>
    easy::try_set_max_lifetime_conn(std::chrono::seconds lifetime)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_MAXLIFETIME_CONN, long(lifetime.count()));
    }


    void
    easy::set_max_redirs(long limit)
    {
        return value_or_throw(try_set_max_redirs(limit));
    }


    std::expected<void, error>
    easy::try_set_max_redirs(long limit)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_MAXREDIRS, limit);
    }


    void
    easy::set_max_recv_speed(curl_off_t speed)
    {
        return value_or_throw(try_set_max_recv_speed(speed));
    }


    std::expected<void, error>
    easy::try_set_max_recv_speed(curl_off_t speed)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_MAX_RECV_SPEED_LARGE, speed);
    }


    void
    easy::set_max_send_speed(curl_off_t speed)
    {
        return value_or_throw(try_set_max_send_speed(speed));
    }


    std::expected<void, error>
    easy::try_set_max_send_speed(curl_off_t speed)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_MAX_SEND_SPEED_LARGE, speed);
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
        return wrap_setopt(raw, CURLOPT_MIMEPOST, data.data());
    }


    void
    easy::unset_mime_post()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_MIMEPOST);
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
        return wrap_setopt(raw, CURLOPT_MIME_OPTIONS, options);
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
        return wrap_setopt(raw, CURLOPT_NETRC, long{level});
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
        return wrap_setopt(raw, CURLOPT_NETRC_FILE, filename.c_str());
    }


    void
    easy::unset_netrc_file()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_NETRC_FILE);
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
        return wrap_setopt(raw, CURLOPT_NEW_DIRECTORY_PERMS, mode);
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
        return wrap_setopt(raw, CURLOPT_NEW_FILE_PERMS, mode);
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
        return wrap_setopt(raw, CURLOPT_NOBODY, no_body);
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
        return wrap_setopt(raw, CURLOPT_NOPROGRESS, no_progress);
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

        auto data_status = wrap_setopt(raw, CURLOPT_OPENSOCKETDATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_OPENSOCKETFUNCTION, &opensocket_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_OPENSOCKETDATA);
        wrap_unsetopt(raw, CURLOPT_OPENSOCKETFUNCTION);
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
        return wrap_setopt(raw, CURLOPT_PASSWORD, password);
    }


    void
    easy::unset_password()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_PASSWORD);
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
        return wrap_setopt(raw, CURLOPT_PORT, long{port});
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
        return wrap_setopt(raw, CURLOPT_POST, enable);
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
    easy::try_set_post_field(std::string_view data)
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
        return wrap_setopt(raw, CURLOPT_POSTFIELDS, data);
    }


    void
    easy::unset_post_field()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_POSTFIELDS);
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
        return wrap_setopt(raw, CURLOPT_POSTFIELDSIZE_LARGE, size);
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
        return wrap_setopt(raw, CURLOPT_READDATA, data_ptr);
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

        auto data_res = wrap_setopt(raw, CURLOPT_READDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = wrap_setopt(raw, CURLOPT_READFUNCTION, &read_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_READDATA);
        wrap_unsetopt(raw, CURLOPT_READFUNCTION);
    }


    void
    easy::set_referer(const std::string& where)
    {
        return value_or_throw(try_set_referer(where));
    }


    std::expected<void, error>
    easy::try_set_referer(const std::string& where)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_REFERER, where);
    }


    void
    easy::unset_referer()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_REFERER);
    }


    void
    easy::set_resume_from(curl_off_t from)
    {
        return value_or_throw(try_set_resume_from(from));
    }


    std::expected<void, error>
    easy::try_set_resume_from(curl_off_t from)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_RESUME_FROM_LARGE, from);
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
        return wrap_setopt(raw, CURLOPT_SSL_VERIFYHOST, enable);
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
        return wrap_setopt(raw, CURLOPT_SSL_VERIFYPEER, enable);
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
        return wrap_setopt(raw, CURLOPT_SSL_VERIFYSTATUS, enable);
    }


    void
    easy::set_stderr(FILE* stream)
    {
        return value_or_throw(try_set_stderr(stream));
    }


    std::expected<void, error>
    easy::try_set_stderr(FILE* stream)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_STDERR, stream);
    }


    void
    easy::unset_stderr()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_STDERR);
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
        return wrap_setopt(raw, CURLOPT_TCP_KEEPALIVE, enable);
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
        return wrap_setopt(raw, CURLOPT_TCP_KEEPCNT, count);
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
        return wrap_setopt(raw, CURLOPT_TCP_KEEPIDLE, long(delay.count()));
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
        return wrap_setopt(raw, CURLOPT_TCP_KEEPINTVL, long(interval.count()));
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
        return wrap_setopt(raw, CURLOPT_TCP_NODELAY, no_delay);
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
        return wrap_setopt(raw, CURLOPT_TIMEOUT, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_TIMEOUT_MS, long(timeout.count()));
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
        return wrap_setopt(raw, CURLOPT_TRANSFERTEXT, enable);
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
        return wrap_setopt(raw, CURLOPT_TRANSFER_ENCODING, enable);
    }


    void
    easy::set_url(const std::string& url_str)
    {
        return value_or_throw(try_set_url(url_str));
    }


    std::expected<void, error>
    easy::try_set_url(const std::string& url_str)
        noexcept
    {
        return wrap_setopt(raw, CURLOPT_URL, url_str);
    }


    void
    easy::unset_url()
        noexcept
    {
        extra_state.url_obj.destroy();
        wrap_unsetopt(raw, CURLOPT_URL);
        wrap_unsetopt(raw, CURLOPT_CURLU);
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
        return wrap_setopt(raw, CURLOPT_USERAGENT, user_agent);
    }


    void
    easy::unset_user_agent()
        noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_USERAGENT);
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
        return wrap_setopt(raw, CURLOPT_USERNAME, username);
    }


    void
    easy::unset_username()
    noexcept
    {
        return wrap_unsetopt(raw, CURLOPT_USERNAME);
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
        return wrap_setopt(raw, CURLOPT_USE_SSL, level);
    }


    void
    easy::set_verbose(bool v)
        noexcept
    {
        std::ignore = wrap_setopt(raw, CURLOPT_VERBOSE, v);
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
        return wrap_setopt(raw, CURLOPT_WILDCARDMATCH, enable);
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
        return wrap_setopt(raw, CURLOPT_WRITEDATA, data_ptr);
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

        auto data_res = wrap_setopt(raw, CURLOPT_WRITEDATA, raw);
        if (!data_res)
            return data_res;

        auto func_res = wrap_setopt(raw, CURLOPT_WRITEFUNCTION, &write_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_WRITEDATA);
        wrap_unsetopt(raw, CURLOPT_WRITEFUNCTION);
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
        return wrap_setopt(raw, CURLOPT_WS_OPTIONS, mask);
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

        auto data_status = wrap_setopt(raw, CURLOPT_XFERINFODATA, raw);
        if (!data_status)
            return data_status;
        auto func_status = wrap_setopt(raw, CURLOPT_XFERINFOFUNCTION, &progress_callback_helper);
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
        wrap_unsetopt(raw, CURLOPT_XFERINFODATA);
        wrap_unsetopt(raw, CURLOPT_XFERINFOFUNCTION);
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
        return wrap_getinfo<curl_socket_t>(raw, CURLINFO_ACTIVESOCKET);
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
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_APPCONNECT_TIME_T
        );
    }


    std::filesystem::path
    easy::get_ca_info()
        const
    {
        return value_or_throw(try_get_ca_info());
    }


    std::expected<std::filesystem::path, error>
    easy::try_get_ca_info()
        const noexcept
    {
        auto result = wrap_getinfo_str(raw, CURLINFO_CAINFO);
        if (!result)
            return std::unexpected{result.error()};
        return std::filesystem::path{std::move(*result)};
    }


    std::filesystem::path
    easy::get_ca_path()
        const
    {
        return value_or_throw(try_get_ca_path());
    }


    std::expected<std::filesystem::path, error>
    easy::try_get_ca_path()
        const noexcept
    {
        auto result = wrap_getinfo_str(raw, CURLINFO_CAPATH);
        if (!result)
            return std::unexpected{result.error()};
        return std::filesystem::path{std::move(*result)};
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
        return wrap_getinfo<long, bool>(raw, CURLINFO_CONDITION_UNMET);
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
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_CONNECT_TIME_T
        );
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_CONN_ID);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_CONTENT_LENGTH_UPLOAD_T);
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
        return wrap_getinfo_str(raw, CURLINFO_CONTENT_TYPE);
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
        return wrap_getinfo<curl_slist*, slist>(raw, CURLINFO_COOKIELIST);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_EARLYDATA_SENT_T);
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
        return wrap_getinfo_str(raw, CURLINFO_EFFECTIVE_METHOD);
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
        return wrap_getinfo_str(raw, CURLINFO_EFFECTIVE_URL);
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
        auto value = wrap_getinfo<curl_off_t>(raw, CURLINFO_FILETIME_T);
        if (!value)
            return std::unexpected{std::move(value.error())};
        return std::chrono::utc_seconds{std::chrono::seconds(*value)};
    }


    std::filesystem::path
    easy::get_ftp_entry_path()
        const
    {
        return value_or_throw(try_get_ftp_entry_path());
    }


    std::expected<std::filesystem::path, error>
    easy::try_get_ftp_entry_path()
        const noexcept
    {
        auto result = wrap_getinfo_str(raw, CURLINFO_FTP_ENTRY_PATH);
        if (!result)
            return std::unexpected{result.error()};
        return std::filesystem::path{std::move(*result)};
    }


    long
    easy::get_header_size()
        const
    {
        return value_or_throw(try_get_header_size());
    }


    std::expected<long, error>
    easy::try_get_header_size()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_HEADER_SIZE);
    }


    long
    easy::get_http_auth_avail()
        const
    {
        return value_or_throw(try_get_http_auth_avail());
    }


    std::expected<long, error>
    easy::try_get_http_auth_avail()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_HTTPAUTH_AVAIL);
    }


#if CURL_AT_LEAST_VERSION(8, 12, 0)

    long
    easy::get_http_auth_used()
        const
    {
        return value_or_throw(try_get_http_auth_used());
    }


    std::expected<long, error>
    easy::try_get_http_auth_used()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_HTTPAUTH_USED);
    }

#endif // CURL_AT_LEAST_VERSION(8, 12, 0)


    long
    easy::get_http_connect_code()
        const
    {
        return value_or_throw(try_get_http_connect_code());
    }


    std::expected<long, error>
    easy::try_get_http_connect_code()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_HTTP_CONNECTCODE);
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
        return wrap_getinfo<long, http_version>(raw, CURLINFO_HTTP_VERSION);
    }


    std::string
    easy::get_local_ip()
        const
    {
        return value_or_throw(try_get_local_ip());
    }


    std::expected<std::string, error>
    easy::try_get_local_ip()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_LOCAL_IP);
    }


    long
    easy::get_local_port()
        const
    {
        return value_or_throw(try_get_local_port());
    }


    std::expected<long, error>
    easy::try_get_local_port()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_LOCAL_PORT);
    }


    std::chrono::microseconds
    easy::get_name_lookup_time()
        const
    {
        return value_or_throw(try_get_name_lookup_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_name_lookup_time()
        const noexcept
    {
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_NAMELOOKUP_TIME_T
        );
    }


    long
    easy::get_num_connects()
        const
    {
        return value_or_throw(try_get_num_connects());
    }


    std::expected<long, error>
    easy::try_get_num_connects()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_NUM_CONNECTS);
    }


    long
    easy::get_os_errno()
        const
    {
        return value_or_throw(try_get_os_errno());
    }


    std::expected<long, error>
    easy::try_get_os_errno()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_OS_ERRNO);
    }



#if CURL_AT_LEAST_VERSION(8, 10, 0)

    std::chrono::microseconds
    get_post_transfer_time()
        const
    {
        return value_or_throw(try_get_post_transfer_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_post_transfer_time()
        const noexcept
    {
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_POSTTRANSFER_TIME_T
        );
    }

#endif // CURL_AT_LEAST_VERSION(8, 10, 0)


    std::chrono::microseconds
    easy::get_pre_transfer_time()
        const
    {
        return value_or_throw(try_get_pre_transfer_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_pre_transfer_time()
        const noexcept
    {
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_PRETRANSFER_TIME_T
        );
    }


    std::string
    easy::get_primary_ip()
        const
    {
        return value_or_throw(try_get_primary_ip());
    }


    std::expected<std::string, error>
    easy::try_get_primary_ip()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_PRIMARY_IP);
    }


    long
    easy::get_primary_port()
        const
    {
        return value_or_throw(try_get_primary_port());
    }


    std::expected<long, error>
    easy::try_get_primary_port()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_PRIMARY_PORT);
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
    easy::get_proxy_auth_avail()
        const
    {
        return value_or_throw(try_get_proxy_auth_avail());
    }


    std::expected<long, error>
    easy::try_get_proxy_auth_avail()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_PROXYAUTH_AVAIL);
    }


#if CURL_AT_LEAST_VERSION(8, 12, 0)

    long
    easy::get_proxy_auth_used()
        const
    {
        return value_or_throw(try_get_proxy_auth_used());
    }


    std::expected<long, error>
    easy::try_get_proxy_auth_used()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_PROXYAUTH_USED);
    }

#endif // CURL_AT_LEAST_VERSION(8, 12, 0)


    CURLproxycode
    easy::get_proxy_error()
        const
    {
        return value_or_throw(try_get_proxy_error());
    }


    std::expected<CURLproxycode, error>
    easy::try_get_proxy_error()
        const noexcept
    {
        return wrap_getinfo<long, CURLproxycode>(raw, CURLINFO_PROXY_ERROR);
    }


    bool
    easy::get_proxy_ssl_verify_result()
        const
    {
        return value_or_throw(try_get_proxy_ssl_verify_result());
    }


    std::expected<bool, error>
    easy::try_get_proxy_ssl_verify_result()
        const noexcept
    {
        return wrap_getinfo<long, bool>(raw, CURLINFO_PROXY_SSL_VERIFYRESULT);
    }


#if CURL_AT_LEAST_VERSION(8, 6, 0)

    std::chrono::microseconds
    easy::get_queue_time()
        const
    {
        return value_or_throw(try_get_queue_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_queue_time()
        const noexcept
    {
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_QUEUE_TIME_T
        );
    }

#endif // CURL_AT_LEAST_VERSION(8, 6, 0)


    long
    easy::get_redirect_count()
        const
    {
        return value_or_throw(try_get_redirect_count());
    }


    std::expected<long, error>
    easy::try_get_redirect_count()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_REDIRECT_COUNT);
    }


    std::chrono::microseconds
    easy::get_redirect_time()
        const
    {
        return value_or_throw(try_get_redirect_time());
    }


    std::expected<std::chrono::microseconds, error>
    easy::try_get_redirect_time()
        const noexcept
    {
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_REDIRECT_TIME_T
        );
    }


    std::string
    easy::get_redirect_url()
        const
    {
        return value_or_throw(try_get_redirect_url());
    }


    std::expected<std::string, error>
    easy::try_get_redirect_url()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_REDIRECT_URL);
    }


    std::string
    easy::get_referer()
        const
    {
        return value_or_throw(try_get_referer());
    }


    std::expected<std::string, error>
    easy::try_get_referer()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_REFERER);
    }


    long
    easy::get_request_size()
        const
    {
        return value_or_throw(try_get_request_size());
    }


    std::expected<long, error>
    easy::try_get_request_size()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_REQUEST_SIZE);
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
        return wrap_getinfo<long>(raw, CURLINFO_RESPONSE_CODE);
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
        return wrap_getinfo<long, std::chrono::seconds>(raw, CURLINFO_RETRY_AFTER);
    }


    long
    easy::get_rtsp_client_cseq()
        const
    {
        return value_or_throw(try_get_rtsp_client_cseq());
    }


    std::expected<long, error>
    easy::try_get_rtsp_client_cseq()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_RTSP_CLIENT_CSEQ);
    }


    long
    easy::get_rtsp_cseq_recv()
        const
    {
        return value_or_throw(try_get_rtsp_cseq_recv());
    }


    std::expected<long, error>
    easy::try_get_rtsp_cseq_recv()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_RTSP_CSEQ_RECV);
    }


    long
    easy::get_rtsp_server_cseq()
        const
    {
        return value_or_throw(try_get_rtsp_server_cseq());
    }


    std::expected<long, error>
    easy::try_get_rtsp_server_cseq()
        const noexcept
    {
        return wrap_getinfo<long>(raw, CURLINFO_RTSP_SERVER_CSEQ);
    }


    std::string
    easy::get_rtsp_session_id()
        const
    {
        return value_or_throw(try_get_rtsp_session_id());
    }


    std::expected<std::string, error>
    easy::try_get_rtsp_session_id()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_RTSP_SESSION_ID);
    }


    std::string
    easy::get_scheme()
        const
    {
        return value_or_throw(try_get_scheme());
    }


    std::expected<std::string, error>
    easy::try_get_scheme()
        const noexcept
    {
        return wrap_getinfo_str(raw, CURLINFO_SCHEME);
    }


#if CURL_AT_LEAST_VERSION(8, 20, 0)

    curl_off_t
    easy::get_size_delivered()
        const
    {
        return value_or_throw(try_get_size_delivered());
    }


    std::expected<curl_off_t, error>
    easy::try_get_size_delivered()
        const noexcept
    {
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_SIZE_DELIVERED);
    }

#endif // CURL_AT_LEAST_VERSION(8, 20, 0)


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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_SIZE_DOWNLOAD_T);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_SIZE_UPLOAD_T);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_SPEED_DOWNLOAD_T);
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
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_SPEED_UPLOAD_T);
    }


    slist
    easy::get_ssl_engines()
        const
    {
        return value_or_throw(try_get_ssl_engines());
    }


    std::expected<slist, error>
    easy::try_get_ssl_engines()
        const noexcept
    {
        auto raw_result = wrap_getinfo<curl_slist*>(raw, CURLINFO_SSL_ENGINES);
        if (!raw_result)
            return std::unexpected{raw_result.error()};
        return slist{*raw_result};
    }


    bool
    easy::get_ssl_verify_result()
        const
    {
        return value_or_throw(try_get_ssl_verify_result());
    }


    std::expected<bool, error>
    easy::try_get_ssl_verify_result()
        const noexcept
    {
        return wrap_getinfo<long, bool>(raw, CURLINFO_SSL_VERIFYRESULT);
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
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_STARTTRANSFER_TIME_T
        );
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
        return wrap_getinfo<curl_off_t, std::chrono::microseconds>(
            raw,
            CURLINFO_TOTAL_TIME_T
        );
    }


#if CURL_AT_LEAST_VERSION(8, 7, 0)

    bool
    easy::get_used_proxy()
        const
    {
        return value_or_throw(try_get_used_proxy());
    }


    std::expected<bool, error>
    easy::try_get_used_proxy()
        const noexcept
    {
        return wrap_getinfo<long, bool>(raw, CURLINFO_USED_PROXY);
    }

#endif // CURL_AT_LEAST_VERSION(8, 7, 0)


#if CURL_AT_LEAST_VERSION(8, 2, 0)

    curl_off_t
    easy::get_xfer_id()
        const
    {
        return value_or_throw(try_get_xfer_id());
    }


    std::expected<curl_off_t, error>
    easy::try_get_xfer_id()
        const noexcept
    {
        return wrap_getinfo<curl_off_t>(raw, CURLINFO_XFER_ID);
    }

#endif //CURL_AT_LEAST_VERSION(8, 2, 0)


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
    easy::closesocket_callback_helper(CURL* handle,
                                      curl_socket_t fd)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.closesocket_func)
                return ez->extra_state.closesocket_func(fd);
            else
                return 1;
        }
        catch (...) {
            return 1;
        }
    }


    int
    easy::debug_callback_helper(CURL* target,
                                curl_infotype type,
                                char *data,
                                std::size_t size,
                                CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.debug_func)
                ez->extra_state.debug_func(target, type, {data, size});
        }
        catch (...) {
        }
        return 0;
    }


    int
    easy::fnmatch_callback_helper(CURL* handle,
                                  const char* pattern,
                                  const char* text)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        if (!ez || !ez->extra_state.fnmatch_func)
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


    std::size_t
    easy::header_callback_helper(char* buffer,
                                 std::size_t size,
                                 std::size_t nitems,
                                 CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.header_func)
                return ez->extra_state.header_func({buffer, size * nitems});
            else
                return CURL_WRITEFUNC_ERROR;
        }
        catch (...) {
            return CURL_WRITEFUNC_ERROR;
        }
    }


    curl_socket_t
    easy::opensocket_callback_helper(CURL* handle,
                                     curlsocktype purpose,
                                     curl_sockaddr* address)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.opensocket_func)
                return ez->extra_state.opensocket_func(purpose, address);
            else
                return CURL_SOCKET_BAD;
        }
        catch (...) {
            return CURL_SOCKET_BAD;
        }
    }


    int
    easy::progress_callback_helper(CURL* handle,
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
    easy::read_callback_helper(char* buf,
                               std::size_t,
                               std::size_t size,
                               CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.read_func)
                return ez->extra_state.read_func({buf, size});
            else
                return CURL_READFUNC_ABORT;
        }
        catch (...) {
            return CURL_READFUNC_ABORT;
        }
    }


    std::size_t
    easy::write_callback_helper(const char* buffer,
                                std::size_t,
                                std::size_t size,
                                CURL* handle)
        noexcept
    {
        easy* ez = get_wrapper(handle);
        try {
            if (ez && ez->extra_state.write_func)
                return ez->extra_state.write_func({buffer, size});
            else
                return CURL_WRITEFUNC_ERROR;
        }
        catch (...) {
            return CURL_WRITEFUNC_ERROR;
        }
    }

} // namespace curl
