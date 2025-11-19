/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_EASY_HPP
#define CURLXX_EASY_HPP

#include <expected>
#include <functional>
#include <span>
#include <string>
#include <tuple>
#include <vector>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"
#include "header.hpp"
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


        // CURLOPT_ABSTRACT_UNIX_SOCKET
        // Path to an abstract Unix domain socket. TODO

        // CURLOPT_ACCEPTTIMEOUT_MS
        // Timeout for waiting for the server's connect back to be accepted. TODO

        // CURLOPT_ACCEPT_ENCODING
        // Accept-Encoding and automatic decompressing data. TODO

        // CURLOPT_ADDRESS_SCOPE
        // IPv6 scope for local addresses. TODO

        // CURLOPT_ALTSVC
        // Specify the Alt-Svc: cache filename. TODO

        // CURLOPT_ALTSVC_CTRL
        // Enable and configure Alt-Svc: treatment. TODO

        // CURLOPT_APPEND
        // Append to remote file. TODO

        // CURLOPT_AUTOREFERER
        // Automatically set Referer: header. TODO

        // CURLOPT_AWS_SIGV4
        // AWS HTTP V4 Signature. TODO

        // CURLOPT_BUFFERSIZE
        // Ask for alternate buffer size. TODO

        // CURLOPT_CAINFO
        // CA cert bundle. TODO

        // CURLOPT_CAINFO_BLOB
        // CA cert bundle memory buffer. TODO

        // CURLOPT_CAPATH
        // Path to CA cert bundle. TODO

        // CURLOPT_CA_CACHE_TIMEOUT
        // Timeout for CA cache. TODO

        // CURLOPT_CERTINFO
        // Extract certificate info. TODO

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
        // Timeout for the connection phase. TODO

        // CURLOPT_CONNECTTIMEOUT_MS
        // Millisecond timeout for the connection phase. TODO

        // CURLOPT_CONNECT_ONLY
        // Only connect, nothing else. TODO

        // CURLOPT_CONNECT_TO
        // Connect to a specific host and port. TODO

        // CURLOPT_COOKIE
        // Cookie(s) to send. TODO

        // CURLOPT_COOKIEFILE
        // File to read cookies from. TODO

        // CURLOPT_COOKIEJAR
        // File to write cookies to. TODO

        // CURLOPT_COOKIELIST
        // Add or control cookies. TODO

        // CURLOPT_COOKIESESSION
        // Start a new cookie session. TODO

        // CURLOPT_COPYPOSTFIELDS
        // Send a POST with this data - and copy it. TODO

        // CURLOPT_CRLF
        // Convert newlines. TODO

        // CURLOPT_CRLFILE
        // Certificate Revocation List. TODO

        // CURLOPT_CURLU
        // Set URL to work on with a URL handle. TODO

        // CURLOPT_CUSTOMREQUEST
        // Custom request/method. TODO

        // CURLOPT_DEBUGDATA
        // Data pointer to pass to the debug callback. TODO

        // CURLOPT_DEBUGFUNCTION
        // Callback for debug information. TODO

        // CURLOPT_DEFAULT_PROTOCOL
        // Default protocol. TODO

        // CURLOPT_DIRLISTONLY
        // List only. TODO

        // CURLOPT_DISALLOW_USERNAME_IN_URL
        // Do not allow username in URL. TODO

        // CURLOPT_DNS_CACHE_TIMEOUT
        // Timeout for DNS cache. TODO

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
        // Request file modification date and time. TODO

        // CURLOPT_FNMATCH_DATA
        // Data pointer to pass to the wildcard matching callback. TODO

        // CURLOPT_FNMATCH_FUNCTION
        // Callback for wildcard matching. TODO


        // CURLOPT_FOLLOWLOCATION
        // Follow HTTP redirects.

        void
        set_follow(bool enable = true);

        std::expected<void, error>
        try_set_follow(bool enable = true)
            noexcept;


        // CURLOPT_FORBID_REUSE
        // Prevent subsequent connections from reusing this.

        void
        set_forbid_reuse(bool forbid = true);

        std::expected<void, error>
        try_set_forbid_reuse(bool forbid = true)
            noexcept;



        // CURLOPT_FRESH_CONNECT
        // Use a new connection. CURLOPT_FRESH_CONNECT

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
        // Timeout for happy eyeballs. TODO

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
        // Control custom headers. TODO

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
        // HTTP server authentication methods. TODO

        // CURLOPT_HTTPGET
        // Do an HTTP GET request. TODO


        // CURLOPT_HTTPHEADER
        // Custom HTTP headers.

        void
        set_http_headers(const std::vector<std::string>& headers);

        std::expected<void, error>
        try_set_http_headers(const std::vector<std::string>& headers)
            noexcept;


        // CURLOPT_HTTPPROXYTUNNEL
        // Tunnel through the HTTP proxy. CURLOPT_HTTPPROXYTUNNEL

        // CURLOPT_HTTP_CONTENT_DECODING
        // Disable Content decoding. TODO

        // CURLOPT_HTTP_TRANSFER_DECODING
        // Disable Transfer decoding. TODO

        // CURLOPT_HTTP_VERSION
        // HTTP version to use. CURLOPT_HTTP_VERSION

        // CURLOPT_IGNORE_CONTENT_LENGTH
        // Ignore Content-Length. TODO

        // CURLOPT_INFILESIZE
        // Size of file to send. CURLOPT_INFILESIZE

        // CURLOPT_INFILESIZE_LARGE
        // Size of file to send. CURLOPT_INFILESIZE_LARGE

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
        // Keep sending on HTTP >= 300 errors. CURLOPT_KEEP_SENDING_ON_ERROR

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
        // Low speed limit to abort transfer. TODO

        // CURLOPT_LOW_SPEED_TIME
        // Time to be below the speed to trigger low speed abort. TODO

        // CURLOPT_MAIL_AUTH
        // Authentication address. TODO

        // CURLOPT_MAIL_FROM
        // Address of the sender. TODO

        // CURLOPT_MAIL_RCPT
        // Address of the recipients. TODO

        // CURLOPT_MAIL_RCPT_ALLOWFAILS
        // Allow RCPT TO command to fail for some recipients. TODO

        // CURLOPT_MAXAGE_CONN
        // Limit the age (idle time) of connections for reuse. TODO

        // CURLOPT_MAXCONNECTS
        // Maximum number of connections in the connection pool. TODO

        // CURLOPT_MAXFILESIZE
        // Maximum file size to get. TODO

        // CURLOPT_MAXFILESIZE_LARGE
        // Maximum file size to get. TODO

        // CURLOPT_MAXLIFETIME_CONN
        // Limit the age (since creation) of connections for reuse. TODO

        // CURLOPT_MAXREDIRS
        // Maximum number of redirects to follow. TODO

        // CURLOPT_MAX_RECV_SPEED_LARGE
        // Cap the download speed to this. TODO

        // CURLOPT_MAX_SEND_SPEED_LARGE
        // Cap the upload speed to this. TODO

        // CURLOPT_MIMEPOST
        // Post/send MIME data. TODO

        // CURLOPT_MIME_OPTIONS
        // Set MIME option flags. TODO

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
        // Filter out hosts from proxy use. CURLOPT_NOPROXY

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
        // Make an HTTP POST. TODO

        // CURLOPT_POSTFIELDSIZE
        // The POST data is this big. TODO

        // CURLOPT_POSTFIELDSIZE_LARGE
        // The POST data is this big. TODO

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
        // Proxy authentication service name. CURLOPT_PROXY_SERVICE_NAME

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

        void
        set_ssl_verify_peer(bool enabled);

        std::expected<void, error>
        try_set_ssl_verify_peer(bool enabled)
            noexcept;


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
        // Set the request target. CURLOPT_REQUEST_TARGET

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
        // Authentication service name. CURLOPT_SERVICE_NAME

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
        // Verify the hostname in the SSL certificate. TODO

        // CURLOPT_SSL_VERIFYPEER
        // Verify the SSL certificate. TODO

        // CURLOPT_SSL_VERIFYSTATUS
        // Verify the SSL certificate's status. TODO

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
        // Enable TCP keep-alive. TODO

        // CURLOPT_TCP_KEEPCNT
        // Maximum number of keep-alive probes. TODO

        // CURLOPT_TCP_KEEPIDLE
        // Idle time before sending keep-alive. TODO

        // CURLOPT_TCP_KEEPINTVL
        // Interval between keep-alive probes. TODO

        // CURLOPT_TCP_NODELAY
        // Disable the Nagle algorithm. TODO

        // CURLOPT_TELNETOPTIONS
        // TELNET options. TODO

        // CURLOPT_TFTP_BLKSIZE
        // TFTP block size. TODO

        // CURLOPT_TFTP_NO_OPTIONS
        // Do not send TFTP options requests. TODO

        // CURLOPT_TIMECONDITION
        // Make a time conditional request. TODO

        // CURLOPT_TIMEOUT
        // Timeout for the entire request. TODO

        // CURLOPT_TIMEOUT_MS
        // Millisecond timeout for the entire request. TODO

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
        // Use text transfer. TODO

        // CURLOPT_TRANSFER_ENCODING
        // Request Transfer-Encoding. TODO

        // CURLOPT_UNIX_SOCKET_PATH
        // Path to a Unix domain socket. TODO

        // CURLOPT_UNRESTRICTED_AUTH
        // Do not restrict authentication to original host. CURLOPT_UNRESTRICTED_AUTH

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
        unset_url();

        std::expected<void, error>
        try_unset_url()
            noexcept;


        // CURLOPT_USERAGENT
        // User-Agent: header.

        void
        set_user_agent(const std::string& ua);

        std::expected<void, error>
        try_set_user_agent(const std::string& ua)
            noexcept;



        // CURLOPT_USERNAME
        // Username. TODO

        // CURLOPT_USERPWD
        // Username and password. TODO

        // CURLOPT_USE_SSL
        // Use TLS/SSL. TODO


        // CURLOPT_VERBOSE
        // Display verbose information.

        void
        set_verbose(bool v = true)
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
        // Set WebSocket options. TODO

        // CURLOPT_XFERINFODATA
        // Data pointer to pass to the progress meter callback. TODO

        // CURLOPT_XFERINFOFUNCTION
        // Callback for progress meter. TODO

        // CURLOPT_XOAUTH2_BEARER
        // OAuth2 bearer token. TODO


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
