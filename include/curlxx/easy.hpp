/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_EASY_HPP
#define CURLXX_EASY_HPP

#include <chrono>
#include <expected>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <tuple>
#include <vector>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"
#include "header.hpp"
#include "mime.hpp"
#include "slist.hpp"


namespace curl {

    class multi;


    class easy : public detail::basic_wrapper<CURL*> {

    public:

        using base_type = detail::basic_wrapper<CURL*>;

        using read_function_t = std::size_t(std::span<char>);
        using write_function_t = std::size_t(std::span<const char>);

        struct extra_state_type {
            std::vector<char> error_buffer;
            std::function<read_function_t> read_callback;
            std::function<write_function_t> write_callback;
            slist header_list;
            slist connect_to_list;
        };

        using state_type = std::tuple<raw_type, extra_state_type>;


    private:

        extra_state_type state;


        void init_error_buffer();
        void link_this();

        static
        std::size_t
        dispatch_read_callback(char* buffer,
                               std::size_t,
                               std::size_t size,
                               CURL* handle)
            noexcept;

        static
        std::size_t
        dispatch_write_callback(const char* buffer,
                                std::size_t,
                                std::size_t size,
                                CURL* handle)
            noexcept;


        template<typename T>
        std::expected<void, error>
        try_setopt(CURLoption opt, T&& arg)
            noexcept;


    public:

        // Inherit constructors.
        using base_type::base_type;


        easy();

        easy(const easy& other);

        easy(easy&& other)
            noexcept;

        easy&
        operator =(const easy& other);

        easy&
        operator =(easy&& other)
            noexcept;


        ~easy()
            noexcept;


        void
        create();

        void
        create(const easy& other);


        void
        destroy()
            noexcept override;


        [[nodiscard]]
        state_type
        release()
            noexcept;


        void
        acquire(state_type new_state)
            noexcept;

        void
        acquire(raw_type new_raw);


        void
        reset();


        void
        pause(bool pause_recv = true,
              bool pause_send = true);

        std::expected<void, error>
        try_pause(bool pause_recv,
                  bool pause_send)
            noexcept;

        void
        unpause();


        void
        perform();

        std::expected<void, error>
        try_perform()
            noexcept;


        /****************************
         * Start of option setters. *
         ****************************/


        // CURLOPT_ABSTRACT_UNIX_SOCKET
        // Path to an abstract Unix domain socket.

        void
        set_abstract_unix_socket(const std::filesystem::path& socket_path);

        std::expected<void, error>
        try_set_abstract_unix_socket(const std::filesystem::path& socket_path)
            noexcept;

        void
        unset_abstract_unix_socket()
            noexcept;


        // CURLOPT_ACCEPTTIMEOUT_MS
        // Timeout for waiting for the server's connect back to be accepted.

        void
        set_accept_timeout(std::chrono::milliseconds timeout);

        std::expected<void, error>
        try_set_accept_timeout(std::chrono::milliseconds timeout)
            noexcept;


        // CURLOPT_ACCEPT_ENCODING
        // Accept-Encoding and automatic decompressing data.

        void
        set_accept_encoding(const std::string& enc);

        std::expected<void, error>
        try_set_accept_encoding(const std::string& enc)
            noexcept;

        void
        unset_accept_encoding()
            noexcept;


        // CURLOPT_ADDRESS_SCOPE
        // IPv6 scope for local addresses.

        void
        set_address_scope(long scope_id);

        std::expected<void, error>
        try_set_address_scope(long scope_id)
            noexcept;


        // CURLOPT_ALTSVC
        // Specify the Alt-Svc: cache filename.

        void
        set_alt_svc(const std::filesystem::path& cache_file);

        std::expected<void, error>
        try_set_alt_svc(const std::filesystem::path& cache_file)
            noexcept;

        void
        unset_alt_svc()
            noexcept;


        // CURLOPT_ALTSVC_CTRL
        // Enable and configure Alt-Svc: treatment.

        void
        set_alt_svc_ctrl(long mask);

        std::expected<void, error>
        try_set_alt_svc_ctrl(long mask)
            noexcept;


        // CURLOPT_APPEND
        // Append to remote file.

        void
        set_append(bool enable);

        std::expected<void, error>
        try_set_append(bool enable)
            noexcept;


        // CURLOPT_AUTOREFERER
        // Automatically set Referer: header.

        void
        set_auto_referer(bool enable);

        std::expected<void, error>
        try_set_auto_referer(bool enable)
            noexcept;


        // CURLOPT_AWS_SIGV4
        // AWS HTTP V4 Signature.

        void
        set_aws_sig_v4(const std::string& arg);

        std::expected<void, error>
        try_set_aws_sig_v4(const std::string& arg)
            noexcept;

        void
        unset_aws_sig_v4()
            noexcept;


        // CURLOPT_BUFFERSIZE
        // Ask for alternate buffer size.

        void
        set_buffer_size(long size);

        std::expected<void, error>
        try_set_buffer_size(long size)
            noexcept;


        // CURLOPT_CAINFO
        // CA cert bundle.

        void
        set_ca_info(const std::filesystem::path& bundle_file);

        std::expected<void, error>
        try_set_ca_info(const std::filesystem::path& bundle_file)
            noexcept;

        void
        unset_ca_info()
            noexcept;


        // CURLOPT_CAINFO_BLOB
        // CA cert bundle memory buffer.
        // TODO: add some convenience wrappers for the blob

        void
        set_ca_info_blob(curl_blob* bundle);

        std::expected<void, error>
        try_set_ca_info_blob(curl_blob* bundle)
            noexcept;


        // CURLOPT_CAPATH
        // Path to CA cert bundle.

        void
        set_ca_path(const std::filesystem::path& bundle_dir);

        std::expected<void, error>
        try_set_ca_path(const std::filesystem::path& bundle_dir)
            noexcept;

        void
        unset_ca_path()
            noexcept;


        // CURLOPT_CA_CACHE_TIMEOUT
        // Timeout for CA cache.

        void
        set_ca_cache_timeout(std::chrono::seconds timeout);

        std::expected<void, error>
        try_set_ca_cache_timeout(std::chrono::seconds timeout)
            noexcept;

        void
        unset_cache_timeout()
            noexcept;


        // CURLOPT_CERTINFO
        // Extract certificate info.

        void
        set_cert_info(bool enable);

        std::expected<void, error>
        try_set_cert_info(bool enable)
            noexcept;


        // CURLOPT_CHUNK_BGN_FUNCTION
        // Callback for wildcard download start of chunk. TODO

        // CURLOPT_CHUNK_DATA
        // Data pointer to pass to the chunk callbacks. TODO

        // CURLOPT_CHUNK_END_FUNCTION
        // Callback for wildcard download end of chunk. TODO


        // CURLOPT_CLOSESOCKETDATA
        // Data pointer to pass to the close socket callback. TODO

        // CURLOPT_CLOSESOCKETFUNCTION
        // Callback for closing socket. TODO


        // CURLOPT_CONNECTTIMEOUT
        // Timeout for the connection phase.

        void
        set_connect_timeout(std::chrono::seconds timeout);

        std::expected<void, error>
        try_set_connect_timeout(std::chrono::seconds timeout)
            noexcept;


        // CURLOPT_CONNECTTIMEOUT_MS
        // Millisecond timeout for the connection phase.

        void
        set_connect_timeout(std::chrono::milliseconds timeout);

        std::expected<void, error>
        try_set_connect_timeout(std::chrono::milliseconds timeout)
            noexcept;


        // CURLOPT_CONNECT_ONLY
        // Only connect, nothing else.

        enum class connect_only : long {
            disable = 0,
            enable = 1,
            websocket = 2,
        };

        void
        set_connect_only(connect_only opt);

        std::expected<void, error>
        try_set_connect_only(connect_only opt)
            noexcept;


        // CURLOPT_CONNECT_TO
        // Connect to a specific host and port.

        void
        set_connect_to(const std::vector<std::string>& hosts);

        std::expected<void, error>
        try_set_connect_to(const std::vector<std::string>& hosts)
            noexcept;


        // CURLOPT_COOKIE
        // Cookie(s) to send.

        void
        set_cookie(const std::string& cookie);

        std::expected<void, error>
        try_set_cookie(const std::string& cookie)
            noexcept;

        void
        unset_cookie()
            noexcept;


        // CURLOPT_COOKIEFILE
        // File to read cookies from.

        void
        set_cookie_file(const std::filesystem::path& cookie_file);

        std::expected<void, error>
        try_set_cookie_file(const std::filesystem::path& cookie_file)
            noexcept;

        void
        unset_cookie_file()
            noexcept;


        // CURLOPT_COOKIEJAR
        // File to write cookies to.

        void
        set_cookie_jar(const std::filesystem::path& jar_file);

        std::expected<void, error>
        try_set_cookie_jar(const std::filesystem::path& jar_file)
            noexcept;

        void
        unset_cookie_jar()
            noexcept;


        // CURLOPT_COOKIELIST
        // Add or control cookies.

        void
        set_cookie_list(const std::string& cookies);

        std::expected<void, error>
        try_set_cookie_list(const std::string& cookies)
            noexcept;

        void
        unset_cookie_list()
            noexcept;


        // CURLOPT_COOKIESESSION
        // Start a new cookie session.

        void
        set_cookie_session(bool start_anew);

        std::expected<void, error>
        try_set_cookie_session(bool start_anew)
            noexcept;


        // CURLOPT_COPYPOSTFIELDS
        // Send a POST with this data - and copy it.

        void
        set_copy_post_field(const std::string& data);

        void
        set_copy_post_field(const void* data,
                            std::size_t size);

        template<typename T,
                 std::size_t E>
        inline
        void
        set_copy_post_field(std::span<T, E> data)
        {
            set_copy_post_field(data.data(), data.size_bytes());
        }

        // alias: _fields -> _field
        template<typename... T>
        inline
        void
        set_copy_post_fields(T... args)
        {
            set_copy_post_field(args...);
        }


        std::expected<void, error>
        try_set_copy_post_field(const std::string& data)
            noexcept;

        std::expected<void, error>
        try_set_copy_post_field(const void* data,
                                std::size_t size)
            noexcept;

        template<typename T,
                 std::size_t E>
        inline
        std::expected<void, error>
        try_set_copy_post_field(std::span<T, E> data)
            noexcept
        {
            return try_set_copy_post_field(data.data(), data.size_bytes());
        }

        // alias: _fields -> _field
        template<typename... T>
        inline
        std::expected<void, error>
        try_set_copy_post_fields(T... args)
            noexcept
        {
            return try_set_copy_post_field(args...);
        }


        void
        unset_copy_post_field()
            noexcept;

        // alias: _fields -> _field
        inline
        void
        unset_copy_post_fields()
            noexcept
        {
            unset_copy_post_field();
        }


        // CURLOPT_CRLF
        // Convert newlines.

        void
        set_crlf(bool convert);

        std::expected<void, error>
        try_set_crlf(bool convert)
            noexcept;


        // CURLOPT_CRLFILE
        // Certificate Revocation List.

        void
        set_crl_file(const std::filesystem::path& crl_file);

        std::expected<void, error>
        try_set_crl_file(const std::filesystem::path& crl_file)
            noexcept;

        void
        unset_crl_file()
            noexcept;


        // CURLOPT_CURLU
        // Set URL to work on with a URL handle. TODO


        // CURLOPT_CUSTOMREQUEST
        // Custom request/method.

        void
        set_custom_request(const std::string& method);

        std::expected<void, error>
        try_set_custom_request(const std::string& method)
            noexcept;

        void
        unset_custom_request()
            noexcept;


        // CURLOPT_DEBUGDATA
        // Data pointer to pass to the debug callback. TODO

        // CURLOPT_DEBUGFUNCTION
        // Callback for debug information. TODO


        // CURLOPT_DEFAULT_PROTOCOL
        // Default protocol.

        void
        set_default_protocol(const std::string& protocol);

        std::expected<void, error>
        try_set_default_protocol(const std::string& protocol)
            noexcept;

        void
        unset_default_protocol()
            noexcept;


        // CURLOPT_DIRLISTONLY
        // List only. TODO


        // CURLOPT_DISALLOW_USERNAME_IN_URL
        // Do not allow username in URL.

        void
        set_disallow_username_in_url(bool disallow);

        std::expected<void, error>
        try_set_disallow_username_in_url(bool disallow)
            noexcept;


        // CURLOPT_DNS_CACHE_TIMEOUT
        // Timeout for DNS cache.

        void
        set_dns_cache_timeout(std::chrono::seconds timeout);

        std::expected<void, error>
        try_set_dns_cache_timeout(std::chrono::seconds timeout)
            noexcept;

        void
        unset_dns_cache_timeout()
            noexcept;


        // CURLOPT_DNS_INTERFACE
        // Bind name resolves to this interface. TODO

        // CURLOPT_DNS_LOCAL_IP4
        // Bind name resolves to this IP4 address. TODO

        // CURLOPT_DNS_LOCAL_IP6
        // Bind name resolves to this IP6 address. TODO

        // CURLOPT_DNS_SERVERS
        // Preferred DNS servers. TODO

        // CURLOPT_DNS_SHUFFLE_ADDRESSES
        // Shuffle addresses before use. TODO

        // CURLOPT_DOH_SSL_VERIFYHOST
        // Verify the hostname in the DoH (DNS-over-HTTPS) SSL certificate. TODO

        // CURLOPT_DOH_SSL_VERIFYPEER
        // Verify the DoH (DNS-over-HTTPS) SSL certificate. TODO

        // CURLOPT_DOH_SSL_VERIFYSTATUS
        // Verify the DoH (DNS-over-HTTPS) SSL certificate's status. TODO

        // CURLOPT_DOH_URL
        // Use this DoH server for name resolves. TODO

        // CURLOPT_ECH
        // Set the configuration for ECH. TODO

        // CURLOPT_ERRORBUFFER
        // Error message buffer. TODO

        // CURLOPT_EXPECT_100_TIMEOUT_MS
        // 100-continue timeout. TODO

        // CURLOPT_FAILONERROR
        // Fail on HTTP 4xx errors. CURLOPT_FAILONERROR


        // CURLOPT_FILETIME
        // Request file modification date and time.

        void
        set_file_time(bool enable);

        std::expected<void, error>
        try_set_file_time(bool enable)
            noexcept;


        // CURLOPT_FNMATCH_DATA
        // Data pointer to pass to the wildcard matching callback. TODO

        // CURLOPT_FNMATCH_FUNCTION
        // Callback for wildcard matching. TODO


        // CURLOPT_FOLLOWLOCATION
        // Follow HTTP redirects.

        void
        set_follow_location(bool enable);

        std::expected<void, error>
        try_set_follow_location(bool enable)
            noexcept;


        // CURLOPT_FORBID_REUSE
        // Prevent subsequent connections from reusing this.

        void
        set_forbid_reuse(bool forbid);

        std::expected<void, error>
        try_set_forbid_reuse(bool forbid)
            noexcept;



        // CURLOPT_FRESH_CONNECT
        // Use a new connection. CURLOPT_FRESH_CONNECT

        void
        set_fresh_connect(bool enable);

        std::expected<void, error>
        try_set_fresh_connect(bool enable)
            noexcept;


        // CURLOPT_FTPPORT
        // Use active FTP. TODO

        // CURLOPT_FTPSSLAUTH
        // Control how to do TLS. TODO

        // CURLOPT_FTP_ACCOUNT
        // Send ACCT command. TODO

        // CURLOPT_FTP_ALTERNATIVE_TO_USER
        // Alternative to USER. TODO

        // CURLOPT_FTP_CREATE_MISSING_DIRS
        // Create missing directories on the remote server. TODO

        // CURLOPT_FTP_FILEMETHOD
        // Specify how to reach files. TODO

        // CURLOPT_FTP_SKIP_PASV_IP
        // Ignore the IP address in the PASV response. TODO

        // CURLOPT_FTP_SSL_CCC
        // Back to non-TLS again after authentication. TODO

        // CURLOPT_FTP_USE_EPRT
        // Use EPRT. TODO

        // CURLOPT_FTP_USE_EPSV
        // Use EPSV. TODO

        // CURLOPT_FTP_USE_PRET
        // Use PRET. TODO

        // CURLOPT_GSSAPI_DELEGATION
        // Disable GSS-API delegation. TODO


        // CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS
        // Timeout for happy eyeballs.

        void
        set_happy_eyeballs_timeout(std::chrono::milliseconds timeout);

        std::expected<void, error>
        try_set_happy_eyeballs_timeout(std::chrono::milliseconds timeout)
            noexcept;


        // CURLOPT_HAPROXYPROTOCOL
        // Send an HAProxy PROXY protocol v1 header. TODO

        // CURLOPT_HAPROXY_CLIENT_IP
        // Spoof the client IP in an HAProxy PROXY protocol v1 header. TODO

        // CURLOPT_HEADER
        // Include the header in the body output. TODO

        // CURLOPT_HEADERDATA
        // Data pointer to pass to the header callback. TODO

        // CURLOPT_HEADERFUNCTION
        // Callback for writing received headers. TODO


        // CURLOPT_HEADEROPT
        // Control custom headers.

        void
        set_header_opt(long mask);

        std::expected<void, error>
        try_set_header_opt(long mask)
            noexcept;


        // CURLOPT_HSTS
        // Set HSTS cache file. TODO

        // CURLOPT_HSTSREADDATA
        // Pass pointer to the HSTS read callback. TODO

        // CURLOPT_HSTSREADFUNCTION
        // Set HSTS read callback. TODO

        // CURLOPT_HSTSWRITEDATA
        // Pass pointer to the HSTS write callback. TODO

        // CURLOPT_HSTSWRITEFUNCTION
        // Set HSTS write callback. TODO

        // CURLOPT_HSTS_CTRL
        // Enable HSTS. TODO

        // CURLOPT_HTTP09_ALLOWED
        // Allow HTTP/0.9 responses. CURLOPT_HTTP09_ALLOWED

        // CURLOPT_HTTP200ALIASES
        // Alternative versions of 200 OK. TODO


        // CURLOPT_HTTPAUTH
        // HTTP server authentication methods.

        void
        set_http_auth(long mask);

        std::expected<void, error>
        try_set_http_auth(long mask)
            noexcept;


        // CURLOPT_HTTPGET
        // Do an HTTP GET request.

        void
        set_http_get(bool use_get);

        std::expected<void, error>
        try_set_http_get(bool use_get)
            noexcept;


        // CURLOPT_HTTPHEADER
        // Custom HTTP headers.

        void
        set_http_headers(const std::vector<std::string>& headers);

        std::expected<void, error>
        try_set_http_headers(const std::vector<std::string>& headers)
            noexcept;

        // TODO: add append_headers too


        // CURLOPT_HTTPPROXYTUNNEL
        // Tunnel through the HTTP proxy. TODO

        // CURLOPT_HTTP_CONTENT_DECODING
        // Disable Content decoding. TODO

        // CURLOPT_HTTP_TRANSFER_DECODING
        // Disable Transfer decoding. TODO


        // CURLOPT_HTTP_VERSION
        // HTTP version to use.

        enum class http_version : long {
            none                = CURL_HTTP_VERSION_NONE,
            v_1_0               = CURL_HTTP_VERSION_1_0,
            v_1_1               = CURL_HTTP_VERSION_1_1,
            v_2_0               = CURL_HTTP_VERSION_2_0,
            v_2tls              = CURL_HTTP_VERSION_2TLS,
            v_2_prior_knowledge = CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE,
            v_3                 = CURL_HTTP_VERSION_3,
            v_3_only            = CURL_HTTP_VERSION_3ONLY,
        };

        void
        set_http_version(http_version ver);

        std::expected<void, error>
        try_set_http_version(http_version ver)
            noexcept;


        // CURLOPT_IGNORE_CONTENT_LENGTH
        // Ignore Content-Length. TODO


        // CURLOPT_INFILESIZE
        // CURLOPT_INFILESIZE_LARGE
        // Size of file to send.

        void
        set_input_file_size(curl_off_t size);

        std::expected<void, error>
        try_set_input_file_size(curl_off_t size)
            noexcept;

        void
        unset_input_file_size()
            noexcept;


        // CURLOPT_INTERFACE
        // Bind connection locally to this. TODO

        // CURLOPT_INTERLEAVEDATA
        // Data pointer to pass to the RTSP interleave callback. TODO

        // CURLOPT_INTERLEAVEFUNCTION
        // Callback for RTSP interleaved data. TODO

        // CURLOPT_IPRESOLVE
        // IP version to use. TODO

        // CURLOPT_ISSUERCERT
        // Issuer certificate. TODO

        // CURLOPT_ISSUERCERT_BLOB
        // Issuer certificate memory buffer. TODO

        // CURLOPT_KEEP_SENDING_ON_ERROR
        // Keep sending on HTTP >= 300 errors. TODO

        // CURLOPT_KEYPASSWD
        // Client key password. TODO

        // CURLOPT_KRBLEVEL
        // Kerberos security level. TODO

        // CURLOPT_LOCALPORT
        // Bind connection locally to this port. TODO

        // CURLOPT_LOCALPORTRANGE
        // Bind connection locally to port range. TODO

        // CURLOPT_LOGIN_OPTIONS
        // Login options. TODO


        // CURLOPT_LOW_SPEED_LIMIT
        // Low speed limit to abort transfer.

        void
        set_low_speed_limit(long limit);

        std::expected<void, error>
        try_set_low_speed_limit(long limit)
            noexcept;


        // CURLOPT_LOW_SPEED_TIME
        // Time to be below the speed to trigger low speed abort.

        void
        set_low_speed_time(std::chrono::seconds time);

        std::expected<void, error>
        try_set_low_speed_time(std::chrono::seconds time)
            noexcept;


        // CURLOPT_MAIL_AUTH
        // Authentication address. TODO

        // CURLOPT_MAIL_FROM
        // Address of the sender. TODO

        // CURLOPT_MAIL_RCPT
        // Address of the recipients. TODO

        // CURLOPT_MAIL_RCPT_ALLOWFAILS
        // Allow RCPT TO command to fail for some recipients. TODO


        // CURLOPT_MAXAGE_CONN
        // Limit the age (idle time) of connections for reuse.

        void
        set_max_age_conn(std::chrono::seconds age);

        std::expected<void, error>
        try_set_max_age_conn(std::chrono::seconds age)
            noexcept;


        // CURLOPT_MAXCONNECTS
        // Maximum number of connections in the connection pool.

        void
        set_max_connects(long amount);

        std::expected<void, error>
        try_set_max_connects(long amount)
            noexcept;


        // CURLOPT_MAXFILESIZE
        // CURLOPT_MAXFILESIZE_LARGE
        // Maximum file size to get.

        void
        set_max_file_size(curl_off_t size);

        std::expected<void, error>
        try_set_max_file_size(curl_off_t size)
            noexcept;


        // CURLOPT_MAXLIFETIME_CONN
        // Limit the age (since creation) of connections for reuse. TODO

        // CURLOPT_MAXREDIRS
        // Maximum number of redirects to follow. TODO

        // CURLOPT_MAX_RECV_SPEED_LARGE
        // Cap the download speed to this. TODO

        // CURLOPT_MAX_SEND_SPEED_LARGE
        // Cap the upload speed to this. TODO


        // CURLOPT_MIMEPOST
        // Post/send MIME data.

        void
        set_mime_post(mime& data);

        std::expected<void, error>
        try_set_mime_post(mime& data)
            noexcept;

        void
        unset_mime_post()
            noexcept;


        // CURLOPT_MIME_OPTIONS
        // Set MIME option flags.

        void
        set_mime_options(long options);

        std::expected<void, error>
        try_set_mime_options(long options)
            noexcept;


        // CURLOPT_NETRC
        // Enable .netrc parsing. TODO

        // CURLOPT_NETRC_FILE
        // .netrc filename. TODO

        // CURLOPT_NEW_DIRECTORY_PERMS
        // Mode for creating new remote directories. TODO

        // CURLOPT_NEW_FILE_PERMS
        // Mode for creating new remote files. TODO

        // CURLOPT_NOBODY
        // Do not get the body contents. TODO

        // CURLOPT_NOPROGRESS
        // Shut off the progress meter. TODO

        // CURLOPT_NOPROXY
        // Filter out hosts from proxy use. TODO

        // CURLOPT_NOSIGNAL
        // Do not install signal handlers. TODO

        // CURLOPT_OPENSOCKETDATA
        // Data pointer to pass to the open socket callback. TODO

        // CURLOPT_OPENSOCKETFUNCTION
        // Callback for socket creation. TODO

        // CURLOPT_PASSWORD
        // Password. TODO

        // CURLOPT_PATH_AS_IS
        // Disable squashing /../ and /./ sequences in the path. TODO

        // CURLOPT_PINNEDPUBLICKEY
        // Set pinned SSL public key . TODO

        // CURLOPT_PIPEWAIT
        // Wait on connection to pipeline on it. TODO

        // CURLOPT_PORT
        // Port number to connect to. TODO


        // CURLOPT_POST
        // Make an HTTP POST.

        void
        set_post(bool enable);

        std::expected<void, error>
        try_set_post(bool enable)
            noexcept;


        // CURLOPT_POSTFIELDS
        // Send a POST with this data - does not copy it.

        void
        set_post_field(const std::string& data);

        void
        set_post_field(const void* data,
                       std::size_t size);

        template<typename T,
                 std::size_t E>
        inline
        void
        set_post_field(std::span<T, E> data)
        {
            set_post_field(data.data(), data.size_bytes());
        }

        // alias: _fields -> _field
        template<typename... T>
        inline
        void
        set_post_fields(T... args)
        {
            set_post_field(args...);
        }


        std::expected<void, error>
        try_set_post_field(const std::string& data)
            noexcept;

        std::expected<void, error>
        try_set_post_field(const void* data,
                           std::size_t size)
            noexcept;

        template<typename T,
                 std::size_t E>
        inline
        std::expected<void, error>
        try_set_post_field(std::span<T, E> data)
            noexcept
        {
            return try_set_post_field(data.data(), data.size_bytes());
        }

        // alias: _fields -> _field
        template<typename... T>
        inline
        std::expected<void, error>
        try_set_post_fields(T... args)
            noexcept
        {
            return try_set_post_field(args...);
        }


        void
        unset_post_field()
            noexcept;

        // alias: _fields -> _field
        inline
        void
        unset_post_fields()
            noexcept
        {
            unset_post_field();
        }


        // CURLOPT_POSTFIELDSIZE
        // CURLOPT_POSTFIELDSIZE_LARGE
        // The POST data is this big.

        void
        set_post_field_size(curl_off_t size);

        std::expected<void, error>
        try_set_post_field_size(curl_off_t size)
            noexcept;



        // CURLOPT_POSTQUOTE
        // Commands to run after transfer. TODO

        // CURLOPT_POSTREDIR
        // How to act on redirects after POST. TODO

        // CURLOPT_PREQUOTE
        // Commands to run just before transfer. TODO

        // CURLOPT_PREREQDATA
        // Data pointer to pass to the CURLOPT_PREREQFUNCTION callback. TODO

        // CURLOPT_PREREQFUNCTION
        // Callback to be called after a connection is established but before a request is made on that connection. TODO

        // CURLOPT_PRE_PROXY
        // Socks proxy to use. TODO

        // CURLOPT_PRIVATE
        // Private pointer to store. TODO

        // CURLOPT_PROGRESSDATA
        // Data pointer to pass to the progress meter callback. TODO

        // CURLOPT_PROTOCOLS_STR
        // Allowed protocols. TODO

        // CURLOPT_PROXY
        // Proxy to use. TODO

        // CURLOPT_PROXYAUTH
        // HTTP proxy authentication methods. TODO

        // CURLOPT_PROXYHEADER
        // Custom HTTP headers sent to proxy. TODO

        // CURLOPT_PROXYPASSWORD
        // Proxy password. TODO

        // CURLOPT_PROXYPORT
        // Proxy port to use. TODO

        // CURLOPT_PROXYTYPE
        // Proxy type. TODO

        // CURLOPT_PROXYUSERNAME
        // Proxy username. TODO

        // CURLOPT_PROXYUSERPWD
        // Proxy username and password. TODO

        // CURLOPT_PROXY_CAINFO
        // Proxy CA cert bundle. TODO

        // CURLOPT_PROXY_CAINFO_BLOB
        // Proxy CA cert bundle memory buffer. TODO

        // CURLOPT_PROXY_CAPATH
        // Path to proxy CA cert bundle. TODO

        // CURLOPT_PROXY_CRLFILE
        // Proxy Certificate Revocation List. TODO

        // CURLOPT_PROXY_ISSUERCERT
        // Proxy issuer certificate. TODO

        // CURLOPT_PROXY_ISSUERCERT_BLOB
        // Proxy issuer certificate memory buffer. TODO

        // CURLOPT_PROXY_KEYPASSWD
        // Proxy client key password. TODO

        // CURLOPT_PROXY_PINNEDPUBLICKEY
        // Set the proxy's pinned SSL public key. TODO

        // CURLOPT_PROXY_SERVICE_NAME
        // Proxy authentication service name. TODO

        // CURLOPT_PROXY_SSLCERT
        // Proxy client cert. TODO

        // CURLOPT_PROXY_SSLCERTTYPE
        // Proxy client cert type. TODO

        // CURLOPT_PROXY_SSLCERT_BLOB
        // Proxy client cert memory buffer. TODO

        // CURLOPT_PROXY_SSLKEY
        // Proxy client key. TODO

        // CURLOPT_PROXY_SSLKEYTYPE
        // Proxy client key type. TODO

        // CURLOPT_PROXY_SSLKEY_BLOB
        // Proxy client key. TODO

        // CURLOPT_PROXY_SSLVERSION
        // Proxy SSL version to use. TODO

        // CURLOPT_PROXY_SSL_CIPHER_LIST
        // Proxy ciphers to use. TODO

        // CURLOPT_PROXY_SSL_OPTIONS
        // Control proxy SSL behavior. TODO

        // CURLOPT_PROXY_SSL_VERIFYHOST
        // Verify the hostname in the proxy SSL certificate. TODO


        // CURLOPT_PROXY_SSL_VERIFYPEER
        // Verify the proxy SSL certificate.

        // CURLOPT_PROXY_TLS13_CIPHERS
        // Proxy TLS 1.3 cipher suites to use. TODO

        // CURLOPT_PROXY_TLSAUTH_PASSWORD
        // Proxy TLS authentication password. TODO

        // CURLOPT_PROXY_TLSAUTH_TYPE
        // Proxy TLS authentication methods. TODO

        // CURLOPT_PROXY_TLSAUTH_USERNAME
        // Proxy TLS authentication username. TODO

        // CURLOPT_PROXY_TRANSFER_MODE
        // Add transfer mode to URL over proxy. TODO

        // CURLOPT_QUICK_EXIT
        // To be set by toplevel tools like "curl" to skip lengthy cleanups when they are about to call exit() anyway. TODO

        // CURLOPT_QUOTE
        // Commands to run before transfer. TODO

        // CURLOPT_RANGE
        // Range requests. TODO

        // CURLOPT_READDATA
        // Data pointer to pass to the read callback. TODO


        // CURLOPT_READFUNCTION
        // Callback for reading data.

        void
        set_read_function(std::function<read_function_t> fn);

        std::expected<void, error>
        try_set_read_function(std::function<read_function_t> fn)
            noexcept;


        // CURLOPT_REDIR_PROTOCOLS_STR
        // Protocols to allow redirects to. TODO

        // CURLOPT_REFERER
        // Referer: header. TODO

        // CURLOPT_REQUEST_TARGET
        // Set the request target. TODO

        // CURLOPT_RESOLVE
        // Provide fixed/fake name resolves. TODO

        // CURLOPT_RESOLVER_START_DATA
        // Data pointer to pass to resolver start callback. TODO

        // CURLOPT_RESOLVER_START_FUNCTION
        // Callback to be called before a new resolve request is started. TODO

        // CURLOPT_RESUME_FROM
        // Resume a transfer. TODO

        // CURLOPT_RESUME_FROM_LARGE
        // Resume a transfer. TODO

        // CURLOPT_RTSP_CLIENT_CSEQ
        // Client CSEQ number. TODO

        // CURLOPT_RTSP_REQUEST
        // RTSP request. TODO

        // CURLOPT_RTSP_SERVER_CSEQ
        // CSEQ number for RTSP Server->Client request. TODO

        // CURLOPT_RTSP_SESSION_ID
        // RTSP session-id. TODO

        // CURLOPT_RTSP_STREAM_URI
        // RTSP stream URI. TODO

        // CURLOPT_RTSP_TRANSPORT
        // RTSP Transport: header. TODO

        // CURLOPT_SASL_AUTHZID
        // SASL authorization identity (identity to act as). TODO

        // CURLOPT_SASL_IR
        // Enable SASL initial response. TODO

        // CURLOPT_SEEKDATA
        // Data pointer to pass to the seek callback. TODO

        // CURLOPT_SEEKFUNCTION
        // Callback for seek operations. TODO

        // CURLOPT_SERVER_RESPONSE_TIMEOUT
        // Timeout for server responses. TODO

        // CURLOPT_SERVER_RESPONSE_TIMEOUT_MS
        // Timeout for server responses. TODO

        // CURLOPT_SERVICE_NAME
        // Authentication service name. TODO

        // CURLOPT_SHARE
        // Share object to use. TODO

        // CURLOPT_SOCKOPTDATA
        // Data pointer to pass to the sockopt callback. TODO

        // CURLOPT_SOCKOPTFUNCTION
        // Callback for sockopt operations. TODO

        // CURLOPT_SOCKS5_AUTH
        // Socks5 authentication methods. TODO

        // CURLOPT_SOCKS5_GSSAPI_NEC
        // Socks5 GSSAPI NEC mode. TODO

        // CURLOPT_SSH_AUTH_TYPES
        // SSH authentication types. TODO

        // CURLOPT_SSH_COMPRESSION
        // Enable SSH compression. TODO

        // CURLOPT_SSH_HOSTKEYDATA
        // Custom pointer to pass to ssh host key callback. TODO

        // CURLOPT_SSH_HOSTKEYFUNCTION
        // Callback for checking host key handling. TODO

        // CURLOPT_SSH_HOST_PUBLIC_KEY_MD5
        // MD5 of host's public key. TODO

        // CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256
        // SHA256 of host's public key. TODO

        // CURLOPT_SSH_KEYDATA
        // Custom pointer to pass to ssh key callback. TODO

        // CURLOPT_SSH_KEYFUNCTION
        // Callback for known hosts handling. TODO

        // CURLOPT_SSH_KNOWNHOSTS
        // Filename with known hosts. TODO

        // CURLOPT_SSH_PRIVATE_KEYFILE
        // Filename of the private key. TODO

        // CURLOPT_SSH_PUBLIC_KEYFILE
        // Filename of the public key. TODO

        // CURLOPT_SSLCERT
        // Client cert. TODO

        // CURLOPT_SSLCERTTYPE
        // Client cert type. TODO

        // CURLOPT_SSLCERT_BLOB
        // Client cert memory buffer. TODO

        // CURLOPT_SSLENGINE
        // Use identifier with SSL engine. TODO

        // CURLOPT_SSLENGINE_DEFAULT
        // Default SSL engine. TODO

        // CURLOPT_SSLKEY
        // Client key. TODO

        // CURLOPT_SSLKEYTYPE
        // Client key type. TODO

        // CURLOPT_SSLKEY_BLOB
        // Client key memory buffer. TODO

        // CURLOPT_SSLVERSION
        // SSL version to use. TODO

        // CURLOPT_SSL_CIPHER_LIST
        // Ciphers to use. TODO

        // CURLOPT_SSL_CTX_DATA
        // Data pointer to pass to the SSL context callback. TODO

        // CURLOPT_SSL_CTX_FUNCTION
        // Callback for SSL context logic. TODO

        // CURLOPT_SSL_EC_CURVES
        // Set key exchange curves. TODO

        // CURLOPT_SSL_ENABLE_ALPN
        // Enable use of ALPN. TODO

        // CURLOPT_SSL_OPTIONS
        // Control SSL behavior. TODO

        // CURLOPT_SSL_SESSIONID_CACHE
        // Disable SSL session-id cache. TODO

        // CURLOPT_SSL_SIGNATURE_ALGORITHMS
        // TLS signature algorithms to use. TODO


        // CURLOPT_SSL_VERIFYHOST
        // Verify the hostname in the SSL certificate.

        void
        set_ssl_verify_host(bool enable);

        std::expected<void, error>
        try_set_ssl_verify_host(bool enable)
            noexcept;


        // CURLOPT_SSL_VERIFYPEER
        // Verify the SSL certificate.

        void
        set_ssl_verify_peer(bool enable);

        std::expected<void, error>
        try_set_ssl_verify_peer(bool enable)
            noexcept;


        // CURLOPT_SSL_VERIFYSTATUS
        // Verify the SSL certificate's status.

        void
        set_ssl_verify_status(bool enable);

        std::expected<void, error>
        try_set_ssl_verify_status(bool enable)
            noexcept;


        // CURLOPT_STDERR
        // Redirect stderr to another stream. TODO

        // CURLOPT_STREAM_DEPENDS
        // This HTTP/2 stream depends on another. TODO

        // CURLOPT_STREAM_DEPENDS_E
        // This HTTP/2 stream depends on another exclusively. TODO

        // CURLOPT_STREAM_WEIGHT
        // Set this HTTP/2 stream's weight. TODO

        // CURLOPT_SUPPRESS_CONNECT_HEADERS
        // Suppress proxy CONNECT response headers from user callbacks. TODO

        // CURLOPT_TCP_FASTOPEN
        // Enable TCP Fast Open. TODO


        // CURLOPT_TCP_KEEPALIVE
        // Enable TCP keep-alive.

        void
        set_tcp_keep_alive(bool enable);

        std::expected<void, error>
        try_set_tcp_keep_alive(bool enable)
            noexcept;


#if CURL_AT_LEAST_VERSION(8, 9, 0)

        // CURLOPT_TCP_KEEPCNT
        // Maximum number of keep-alive probes.

        void
        set_tcp_keep_cnt(long count);

        std::expected<void, error>
        try_set_tcp_keep_cnt(long count)
            noexcept;

#endif


        // CURLOPT_TCP_KEEPIDLE
        // Idle time before sending keep-alive.

        void
        set_tcp_keep_idle(std::chrono::seconds delay);

        std::expected<void, error>
        try_set_tcp_keep_idle(std::chrono::seconds delay)
            noexcept;


        // CURLOPT_TCP_KEEPINTVL
        // Interval between keep-alive probes.

        void
        set_tcp_keep_intvl(std::chrono::seconds interval);

        std::expected<void, error>
        try_set_tcp_keep_intvl(std::chrono::seconds interval)
            noexcept;


        // CURLOPT_TCP_NODELAY
        // Disable the Nagle algorithm.

        void
        set_tcp_no_delay(bool no_delay);

        std::expected<void, error>
        try_set_tcp_no_delay(bool no_delay)
            noexcept;


        // CURLOPT_TELNETOPTIONS
        // TELNET options. TODO

        // CURLOPT_TFTP_BLKSIZE
        // TFTP block size. TODO

        // CURLOPT_TFTP_NO_OPTIONS
        // Do not send TFTP options requests. TODO

        // CURLOPT_TIMECONDITION
        // Make a time conditional request. TODO


        // CURLOPT_TIMEOUT
        // Timeout for the entire request.

        void
        set_timeout(std::chrono::seconds timeout);

        std::expected<void, error>
        try_set_timeout(std::chrono::seconds timeout)
            noexcept;


        // CURLOPT_TIMEOUT_MS
        // Millisecond timeout for the entire request.

        void
        set_timeout(std::chrono::milliseconds timeout);

        std::expected<void, error>
        try_set_timeout(std::chrono::milliseconds timeout)
            noexcept;


        // CURLOPT_TIMEVALUE
        // Time value for the time conditional request. TODO

        // CURLOPT_TIMEVALUE_LARGE
        // Time value for the time conditional request. TODO

        // CURLOPT_TLS13_CIPHERS
        // TLS 1.3 cipher suites to use. TODO

        // CURLOPT_TLSAUTH_PASSWORD
        // TLS authentication password. TODO

        // CURLOPT_TLSAUTH_TYPE
        // TLS authentication methods. TODO

        // CURLOPT_TLSAUTH_USERNAME
        // TLS authentication username. TODO

        // CURLOPT_TRAILERDATA
        // Custom pointer passed to the trailing headers callback. TODO

        // CURLOPT_TRAILERFUNCTION
        // Set callback for sending trailing headers. TODO


        // CURLOPT_TRANSFERTEXT
        // Use text transfer.

        void
        set_transfer_text(bool enable);

        std::expected<void, error>
        try_set_transfer_text(bool enable)
            noexcept;


        // CURLOPT_TRANSFER_ENCODING
        // Request Transfer-Encoding.

        void
        set_transfer_encoding(bool enable);

        std::expected<void, error>
        try_set_transfer_encoding(bool enable)
            noexcept;


        // CURLOPT_UNIX_SOCKET_PATH
        // Path to a Unix domain socket. TODO

        // CURLOPT_UNRESTRICTED_AUTH
        // Do not restrict authentication to original host. TODO

        // CURLOPT_UPKEEP_INTERVAL_MS
        // Sets the interval at which connection upkeep are performed. TODO

        // CURLOPT_UPLOAD
        // Upload data. TODO

        // CURLOPT_UPLOAD_BUFFERSIZE
        // Set upload buffer size. TODO

        // CURLOPT_UPLOAD_FLAGS
        // Set upload flags. TODO


        // CURLOPT_URL
        // URL to work on.

        void
        set_url(const std::string& url);

        std::expected<void, error>
        try_set_url(const std::string& url)
            noexcept;

        void
        unset_url()
            noexcept;


        // CURLOPT_USERAGENT
        // User-Agent: header.

        void
        set_user_agent(const std::string& ua);

        std::expected<void, error>
        try_set_user_agent(const std::string& ua)
            noexcept;

        void
        unset_user_agent()
            noexcept;


        // CURLOPT_USERNAME
        // Username. TODO

        // CURLOPT_USERPWD
        // Username and password. TODO


        // CURLOPT_USE_SSL
        // Use TLS/SSL.

        enum class ssl_level : long {
            none = CURLUSESSL_NONE ,
            optional = CURLUSESSL_TRY,
            control = CURLUSESSL_CONTROL,
            all = CURLUSESSL_ALL,
        };

        void
        set_use_ssl(ssl_level level);

        std::expected<void, error>
        try_set_use_ssl(ssl_level level)
            noexcept;


        // CURLOPT_VERBOSE
        // Display verbose information.

        void
        set_verbose(bool v)
            noexcept;


        // CURLOPT_WILDCARDMATCH
        // Transfer multiple files according to a filename pattern. TODO

        // CURLOPT_WRITEDATA
        // Data pointer to pass to the write callback. TODO


        // CURLOPT_WRITEFUNCTION
        // Callback for writing data.

        void
        set_write_function(std::function<write_function_t> fn);

        std::expected<void, error>
        try_set_write_function(std::function<write_function_t> fn)
            noexcept;


        // CURLOPT_WS_OPTIONS
        // Set WebSocket options.

        void
        set_ws_options(long mask);

        std::expected<void, error>
        try_set_ws_options(long mask)
            noexcept;


        // CURLOPT_XFERINFODATA
        // Data pointer to pass to the progress meter callback. TODO

        // CURLOPT_XFERINFOFUNCTION
        // Callback for progress meter. TODO

        // CURLOPT_XOAUTH2_BEARER
        // OAuth2 bearer token. TODO


        /**************************
         * End of option setters. *
         **************************/


        /**************************
         * Start of info getters. *
         **************************/


        /*
          TODO

          CURLINFO_ACTIVESOCKET
          The session's active socket. See CURLINFO_ACTIVESOCKET

          CURLINFO_APPCONNECT_TIME
          The time it took from the start until the SSL connect/handshake with the remote host was completed as a double in number of seconds.

          CURLINFO_APPCONNECT_TIME_T
          The time it took from the start until the SSL connect/handshake with the remote host was completed in number of microseconds. (Added in 7.60.0) See CURLINFO_APPCONNECT_TIME_T

          CURLINFO_CAINFO
          Get the default value for CURLOPT_CAINFO. See CURLINFO_CAINFO

          CURLINFO_CAPATH
          Get the default value for CURLOPT_CAPATH. See CURLINFO_CAPATH

          CURLINFO_CERTINFO
          Certificate chain. See CURLINFO_CERTINFO

          CURLINFO_CONDITION_UNMET
          Whether or not a time conditional was met or 304 HTTP response. See CURLINFO_CONDITION_UNMET

          CURLINFO_CONNECT_TIME
          The time it took from the start until the connect to the remote host (or proxy) was completed. As a double. See CURLINFO_CONNECT_TIME

          CURLINFO_CONNECT_TIME_T
          The time it took from the start until the connect to the remote host (or proxy) was completed. In microseconds. See CURLINFO_CONNECT_TIME_T.

          CURLINFO_CONN_ID
          The ID of the last connection used by the transfer. (Added in 8.2.0) See CURLINFO_CONN_ID

          CURLINFO_CONTENT_LENGTH_DOWNLOAD_T
          Content length from the Content-Length header. See CURLINFO_CONTENT_LENGTH_DOWNLOAD_T

          CURLINFO_CONTENT_LENGTH_UPLOAD_T
          Upload size. See CURLINFO_CONTENT_LENGTH_UPLOAD_T

          CURLINFO_CONTENT_TYPE
          Content type from the Content-Type: header. We recommend using curl_easy_header instead. See CURLINFO_CONTENT_TYPE

          CURLINFO_COOKIELIST
          List of all known cookies. See CURLINFO_COOKIELIST

          CURLINFO_EARLYDATA_SENT_T
          Amount of TLS early data sent (in number of bytes) when CURLSSLOPT_EARLYDATA is enabled.

          CURLINFO_EFFECTIVE_METHOD
          Last used HTTP method. See CURLINFO_EFFECTIVE_METHOD

          CURLINFO_EFFECTIVE_URL
          Last used URL. See CURLINFO_EFFECTIVE_URL

          CURLINFO_FILETIME
          Remote time of the retrieved document. See CURLINFO_FILETIME

          CURLINFO_FILETIME_T
          Remote time of the retrieved document. See CURLINFO_FILETIME_T

          CURLINFO_FTP_ENTRY_PATH
          The entry path after logging in to an FTP server. See CURLINFO_FTP_ENTRY_PATH

          CURLINFO_HEADER_SIZE
          Number of bytes of all headers received. See CURLINFO_HEADER_SIZE

          CURLINFO_HTTPAUTH_AVAIL
          Available HTTP authentication methods. See CURLINFO_HTTPAUTH_AVAIL

          CURLINFO_HTTPAUTH_USED
          Used HTTP authentication method. See CURLINFO_HTTPAUTH_USED

          CURLINFO_HTTP_CONNECTCODE
          Last proxy CONNECT response code. See CURLINFO_HTTP_CONNECTCODE

          CURLINFO_HTTP_VERSION
          The http version used in the connection. See CURLINFO_HTTP_VERSION

          CURLINFO_LOCAL_IP
          Source IP address of the last connection. See CURLINFO_LOCAL_IP

          CURLINFO_LOCAL_PORT
          Source port number of the last connection. See CURLINFO_LOCAL_PORT

          CURLINFO_NAMELOOKUP_TIME
          Time from start until name resolving completed as a double. See CURLINFO_NAMELOOKUP_TIME

          CURLINFO_NAMELOOKUP_TIME_T
          Time from start until name resolving completed in number of microseconds. See CURLINFO_NAMELOOKUP_TIME_T

          CURLINFO_NUM_CONNECTS
          Number of new successful connections used for previous transfer. See CURLINFO_NUM_CONNECTS

          CURLINFO_OS_ERRNO
          The errno from the last failure to connect. See CURLINFO_OS_ERRNO

          CURLINFO_POSTTRANSFER_TIME_T
          The time it took from the start until the last byte is sent by libcurl. In microseconds. (Added in 8.10.0) See CURLINFO_POSTTRANSFER_TIME_T

          CURLINFO_PRETRANSFER_TIME
          The time it took from the start until the file transfer is just about to begin. This includes all pre-transfer commands and negotiations that are specific to the particular protocol(s) involved. See CURLINFO_PRETRANSFER_TIME

          CURLINFO_PRETRANSFER_TIME_T
          The time it took from the start until the file transfer is just about to begin. This includes all pre-transfer commands and negotiations that are specific to the particular protocol(s) involved. In microseconds. See CURLINFO_PRETRANSFER_TIME_T

          CURLINFO_PRIMARY_IP
          Destination IP address of the last connection. See CURLINFO_PRIMARY_IP

          CURLINFO_PRIMARY_PORT
          Destination port of the last connection. See CURLINFO_PRIMARY_PORT

          CURLINFO_PRIVATE
          User's private data pointer. See CURLINFO_PRIVATE

          CURLINFO_PROXYAUTH_AVAIL
          Available HTTP proxy authentication methods. See CURLINFO_PROXYAUTH_AVAIL

          CURLINFO_PROXYAUTH_USED
          Used HTTP proxy authentication methods. See CURLINFO_PROXYAUTH_USED

          CURLINFO_PROXY_ERROR
          Detailed proxy error. See CURLINFO_PROXY_ERROR

          CURLINFO_PROXY_SSL_VERIFYRESULT
          Proxy certificate verification result. See CURLINFO_PROXY_SSL_VERIFYRESULT

          CURLINFO_QUEUE_TIME_T
          The time during which the transfer was held in a waiting queue before it could start for real in number of microseconds. (Added in 8.6.0) See CURLINFO_QUEUE_TIME_T

          CURLINFO_REDIRECT_COUNT
          Total number of redirects that were followed. See CURLINFO_REDIRECT_COUNT

          CURLINFO_REDIRECT_TIME
          The time it took for all redirection steps include name lookup, connect, pretransfer and transfer before final transaction was started. So, this is zero if no redirection took place. As a double. See CURLINFO_REDIRECT_TIME

          CURLINFO_REDIRECT_TIME_T
          The time it took for all redirection steps include name lookup, connect, pretransfer and transfer before final transaction was started. So, this is zero if no redirection took place. In number of microseconds. See CURLINFO_REDIRECT_TIME_T

          CURLINFO_REDIRECT_URL
          URL a redirect would take you to, had you enabled redirects. See CURLINFO_REDIRECT_URL

          CURLINFO_REFERER
          Referrer header. See CURLINFO_REFERER

          CURLINFO_REQUEST_SIZE
          Number of bytes sent in the issued HTTP requests. See CURLINFO_REQUEST_SIZE

          CURLINFO_RESPONSE_CODE
          Last received response code. See CURLINFO_RESPONSE_CODE

          CURLINFO_RETRY_AFTER
          The value from the Retry-After header. See CURLINFO_RETRY_AFTER

          CURLINFO_RTSP_CLIENT_CSEQ
          The RTSP client CSeq that is expected next. See CURLINFO_RTSP_CLIENT_CSEQ

          CURLINFO_RTSP_CSEQ_RECV
          RTSP CSeq last received. See CURLINFO_RTSP_CSEQ_RECV

          CURLINFO_RTSP_SERVER_CSEQ
          The RTSP server CSeq that is expected next. See CURLINFO_RTSP_SERVER_CSEQ

          CURLINFO_RTSP_SESSION_ID
          RTSP session ID. See CURLINFO_RTSP_SESSION_ID

          CURLINFO_SCHEME
          The scheme used for the connection. See CURLINFO_SCHEME

          CURLINFO_SIZE_DOWNLOAD_T
          Number of bytes downloaded. See CURLINFO_SIZE_DOWNLOAD_T

          CURLINFO_SIZE_UPLOAD_T
          Number of bytes uploaded. See CURLINFO_SIZE_UPLOAD_T

          CURLINFO_SPEED_DOWNLOAD_T
          Average download speed. See CURLINFO_SPEED_DOWNLOAD_T

          CURLINFO_SPEED_UPLOAD_T
          Average upload speed in number of bytes per second. See CURLINFO_SPEED_UPLOAD_T

          CURLINFO_SSL_ENGINES
          A list of OpenSSL crypto engines. See CURLINFO_SSL_ENGINES

          CURLINFO_SSL_VERIFYRESULT
          Certificate verification result. See CURLINFO_SSL_VERIFYRESULT

          CURLINFO_STARTTRANSFER_TIME
          The time it took from the start until the first byte is received by libcurl. As a double. See CURLINFO_STARTTRANSFER_TIME

          CURLINFO_STARTTRANSFER_TIME_T
          The time it took from the start until the first byte is received by libcurl. In microseconds. See CURLINFO_STARTTRANSFER_TIME_T

          CURLINFO_TLS_SESSION
          (Deprecated) TLS session info that can be used for further processing. See CURLINFO_TLS_SESSION. Use CURLINFO_TLS_SSL_PTR instead.

          CURLINFO_TLS_SSL_PTR
          TLS session info that can be used for further processing. See CURLINFO_TLS_SSL_PTR

          CURLINFO_TOTAL_TIME
          Total time of previous transfer. See CURLINFO_TOTAL_TIME

          CURLINFO_TOTAL_TIME_T
          Total time of previous transfer. See CURLINFO_TOTAL_TIME_T

          CURLINFO_USED_PROXY
          Whether the proxy was used (Added in 8.7.0). See CURLINFO_USED_PROXY

          CURLINFO_XFER_ID
          The ID of the transfer. (Added in 8.2.0) See CURLINFO_XFER_ID
        */


        /************************
         * End of info getters. *
         ************************/


        header
        get_header(const std::string& name,
                   std::size_t index = 0,
                   unsigned origin = CURLH_HEADER,
                   int request = -1)
            const;


        std::expected<header, error>
        try_get_header(const std::string& name,
                       std::size_t index = 0,
                       unsigned origin = CURLH_HEADER,
                       int request = -1)
            const noexcept;


        [[nodiscard]]
        static
        easy*
        get_wrapper(CURL* h)
            noexcept;


        friend class multi;

    }; // class easy

} // namespace curl

#endif
