/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef CURLXX_MULTI_HPP
#define CURLXX_MULTI_HPP

#include <chrono>
#include <cstddef>
#include <expected>
#include <functional>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <curl/curl.h>

#include "basic_wrapper.hpp"
#include "error.hpp"


namespace curl {

    struct easy;

    /// A non-owning wrapper for curl_pushheaders, only used by the pushheaders callback.
    struct pushheaders {

        pushheaders(curl_pushheaders* raw_,
                    std::size_t size_)
            noexcept;

        curl_pushheaders*
        data()
            const noexcept;

        bool
        empty()
            const noexcept;

        std::size_t
        size()
            const noexcept;

        std::optional<std::string_view>
        operator [](const char* name)
            const noexcept;

        std::optional<std::string_view>
        operator [](const std::string& name)
            const noexcept;

        std::optional<std::string_view>
        operator [](std::size_t num)
            const noexcept;

    private:

        curl_pushheaders* const raw;
        const std::size_t num_headers;

    }; // struct pushheaders


    struct multi : detail::basic_wrapper<CURLM*> {

        using base_type = detail::basic_wrapper<CURLM*>;


#if CURL_AT_LEAST_VERSION(8, 17, 0)

        using notify_callback_signature =
            void (
                unsigned type,
                easy* handle,
                CURL* raw_handle
            );

        using notify_function_t = std::move_only_function<notify_callback_signature>;


#endif // CURL_AT_LEAST_VERSION(8, 17, 0)

        using push_callback_signature =
            int (
                easy* parent_handle,
                CURL *raw_parent_handle,
                CURL *raw_new_handle,
                const pushheaders& headers
            );

        using push_function_t = std::move_only_function<push_callback_signature>;


        using socket_callback_signature =
            int (
                easy* handle,
                CURL* raw_handle,
                curl_socket_t fd,
                int what,
                void* socket_data
            );

        using socket_function_t = std::move_only_function<socket_callback_signature>;


        using timer_callback_signature = int (std::chrono::milliseconds timeout);

        using timer_function_t = std::move_only_function<timer_callback_signature>;


        struct extra_state_type {
#if CURL_AT_LEAST_VERSION(8, 17, 0)
            notify_function_t notify_func;
#endif // CURL_AT_LEAST_VERSION(8, 17, 0)
            push_function_t   push_func;
            socket_function_t socket_func;
            timer_function_t  timer_func;
        };

        using state_type = std::tuple<base_type::state_type, extra_state_type>;


        /// Default constructor.

        multi();

        /// Empty constructor.

        constexpr
        inline
        multi(std::nullptr_t)
            noexcept
        {}

        /// Wrapping constructor.

        explicit
        multi(CURLM* handle);

        /// Move constructor.

        multi(multi&& other)
            noexcept = default;

        /// Move assignment.

        multi&
        operator =(multi&& other)
            noexcept = default;

        /// Destructor.

        ~multi()
            noexcept;


        /**
         * Calls `curl_multi_init()`: create a multi handle.
         *
         * This is called by the default construtor.
         */
        void
        create();

        /**
         * Wrap a given handle.
         *
         * This is called by the wrapping constructor. The handle is now owned by the multi
         * object.
         */
        void
        create(CURLM* handle);

        /**
         * Calls `curl_multi_cleanup()`: close down a multi session.
         *
         * The multi object is now invalid until until you call `create()` on it.
         */

        void
        destroy()
            noexcept override;


        void
        acquire(state_type new_state)
            noexcept;

        void
        acquire(raw_type new_raw)
            noexcept;



        [[nodiscard]]
        state_type
        release()
            noexcept;

        /// Wrapper for `curl_multi_add_handle()`: add an easy handle to a multi session.

        void
        add(easy& ez);

        std::expected<void, error>
        try_add(easy& ez)
            noexcept;

        /**
         * Wrapper for `curl_multi_remove_handle()`: remove an easy handle from a multi session.
         */

        void
        remove(easy& ez);

        std::expected<void, error>
        try_remove(easy& ez)
            noexcept;

        /// Wrapper for `curl_multi_perform()`: run all transfers until it would block.

        unsigned
        perform();

        std::expected<unsigned, error>
        try_perform()
            noexcept;


        struct msg_done {
            CURL* raw_handle;
            easy* handle; ///< Might be null, for internal handles with no wrapper.
            CURLcode result;
        };

        /**
         * Wrapper for `curl_multi_info_read()`: read multi stack information.
         */

        std::optional<msg_done>
        info_read()
            noexcept;

        std::optional<msg_done>
        info_read(int& pending)
            noexcept;

        /**
         * Convenience function to obtain all pending messages with `info_read()`.
         */

        std::vector<msg_done>
        get_done();

        /// Wrapper for `curl_multi_poll()`: poll on all easy handles in a multi handle.

        int
        poll(std::span<curl_waitfd> extra_fds,
             std::chrono::milliseconds timeout);

        int
        poll(std::span<curl_waitfd> extra_fds,
             std::chrono::milliseconds timeout,
             std::stop_token stopper);

        int
        poll(std::chrono::milliseconds timeout);

        int
        poll(std::chrono::milliseconds timeout,
             std::stop_token stopper);

        std::expected<int, error>
        try_poll(std::span<curl_waitfd> extra_fds,
                 std::chrono::milliseconds timeout)
            noexcept;

        std::expected<int, error>
        try_poll(std::span<curl_waitfd> extra_fds,
                 std::chrono::milliseconds timeout,
                 std::stop_token stopper)
            noexcept;

        std::expected<int, error>
        try_poll(std::chrono::milliseconds timeout)
            noexcept;

        std::expected<int, error>
        try_poll(std::chrono::milliseconds timeout,
                 std::stop_token stopper)
            noexcept;

        /// Wrapper for `curl_multi_wakeup()`: wake up a sleeping poll call.

        void
        wakeup();

        std::expected<void, error>
        try_wakeup()
            noexcept;

        /// Wrapper for `curl_multi_wait()`: poll on all easy handles in a multi handle.

        int
        wait(std::span<curl_waitfd> extra_fds,
             std::chrono::milliseconds timeout);

        std::expected<int, error>
        try_wait(std::span<curl_waitfd> extra_fds,
                 std::chrono::milliseconds timeout)
            noexcept;

#if CURL_AT_LEAST_VERSION(8, 8, 0)

        /**
         * Wrapper for `curl_multi_waitfds()`: extract file descriptor information from a multi
         * handle.
         */

        unsigned
        waitfds(std::span<curl_waitfd> ufds);

        std::expected<unsigned, error>
        try_waitfds(std::span<curl_waitfd> ufds)
            noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 8, 0)

        /**
         * Wrapper for `curl_multi_fdset()`: extract file descriptor information from a multi
         * handle.
         */

        void
        fdset(fd_set& read_set,
              fd_set& write_set,
              fd_set& except_set,
              int& max_fd)
            const;

        std::expected<void, error>
        try_fdset(fd_set& read_set,
                  fd_set& write_set,
                  fd_set& except_set,
                  int& max_fd)
            const noexcept;

#if CURL_AT_LEAST_VERSION(8, 4, 0)

        /**
         * Wrapper for `curl_multi_get_handles()`: return all added easy handles.
         *
         * Only wrapped handles are returned.
         */

        std::vector<easy*>
        get_handles()
            const;

        std::expected<std::vector<easy*>, error>
        try_get_handles()
            const noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 4, 0)

#if CURL_AT_LEAST_VERSION(8, 17, 0)

        /// Wrapper for `curl_multi_notify_disable()`: disable a notification type.

        void
        notify_disable(unsigned type);

        std::expected<void, error>
        try_notify_disable(unsigned type)
            noexcept;

        /// Wrapper for `curl_multi_notify_enable()`: enable a notification type.

        void
        notify_enable(unsigned type);

        std::expected<void, error>
        try_notify_enable(unsigned type)
            noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 17, 0)

        /// Wrapper for `curl_multi_assign()`: set data to associate with an internal socket.

        void
        assign(curl_socket_t fd,
               void* data);

        std::expected<void, error>
        try_assign(curl_socket_t fd,
                   void* data)
            noexcept;

        /// Wrapper for `curl_multi_socket_action()`: read/write available data given an action.

        int
        socket_action(curl_socket_t fd,
                      int event);

        std::expected<int, error>
        try_socket_action(curl_socket_t fd,
                          int event)
            noexcept;

        /// Wrapper for `curl_multi_timeout()`: how long to wait for action before proceeding.

        std::chrono::milliseconds
        get_timeout();

        std::expected<std::chrono::milliseconds, error>
        try_get_timeout()
            noexcept;

        /* ---------------------------------------- */
        /* Start of option setters.                 */
        /*                                          */
        /* They wrap calls to `curl_multi_setopt()` */
        /* ---------------------------------------- */

        /// Wrapper for `CURLMOPT_MAXCONNECTS`: Size of connection cache.

        void
        set_max_connections(long n);

        std::expected<void, error>
        try_set_max_connections(long n)
            noexcept;

        /// Wrapper for `CURLMOPT_MAX_CONCURRENT_STREAMS`: Max concurrent streams for http2.

        void
        set_max_concurrent_streams(long n);

        std::expected<void, error>
        try_set_max_concurrent_streams(long n)
            noexcept;

        /**
         * Wrapper for `CURLMOPT_MAX_HOST_CONNECTIONS`: Max number of connections to a single
         * host.
         */

        void
        set_max_host_connections(long n);

        std::expected<void, error>
        try_set_max_host_connections(long n)
            noexcept;

        /// Wrapper for `CURLMOPT_MAX_TOTAL_CONNECTIONS`: Max simultaneously open connections.

        void
        set_max_total_connections(long n);

        std::expected<void, error>
        try_set_max_total_connections(long n)
            noexcept;

#if CURL_AT_LEAST_VERSION(8, 16, 0)

        /// Wrapper for `CURLMOPT_NETWORK_CHANGED`: Signal that the network has changed.

        void
        set_network_changed(long mask);

        std::expected<void, error>
        try_set_network_changed(long mask)
            noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)

#if CURL_AT_LEAST_VERSION(8, 17, 0)

        /**
         * Wrapper for `CURLMOPT_NOTIFYFUNCTION` and `CURLMOPT_NOTIFYDATA`: Callback that
         * receives notifications.
        */

        void
        set_notify_function(notify_function_t func)
            noexcept;

        void
        unset_notify_function()
            noexcept;

#endif // CURL_AT_LEAST_VERSION(8, 17, 0)


        /// Wrapper for `CURLMOPT_PIPELINING`: Enable HTTP multiplexing.

        void
        set_pipelining(long mask);

        std::expected<void, error>
        try_set_pipelining(long mask)
            noexcept;

        /**
         * Wrapper for `CURLMOPT_PUSHFUNCTION` and `CURLMOPT_PUSHDATA`: Callback that approves
         * or denies server pushes.
         */

        void
        set_push_function(push_function_t func)
            noexcept;

        void
        unset_push_function()
            noexcept;

        /**
         * Wrapper for `CURLMOPT_SOCKETFUNCTION` and `CURLMOPT_SOCKETDATA`: Callback informed
         * about what to wait for.
         */

        void
        set_socket_function(socket_function_t func)
            noexcept;

        void
        unset_socket_function()
            noexcept;

        /**
         * Wrapper for `CURLMOPT_TIMERFUNCTION` and `CURLMOPT_TIMERDATA`: Callback to receive
         * timeout values.
         */

        void
        set_timer_function(timer_function_t func)
            noexcept;

        void
        unset_timer_function()
            noexcept;

        /* ---------------------- */
        /* End of option setters. */
        /* ---------------------- */

#if CURL_AT_LEAST_VERSION(8, 16, 0)

        // Begin of getters

        /// Wrapper for `CURLMINFO_XFERS_CURRENT`.

        curl_off_t
        get_xfers_current()
            const;

        std::expected<curl_off_t, error>
        try_get_xfers_current()
            const noexcept;

        /// Wrapper for `CURLMINFO_XFERS_RUNNING`.

        curl_off_t
        get_xfers_running()
            const;

        std::expected<curl_off_t, error>
        try_get_xfers_running()
            const noexcept;

        /// Wrapper for `CURLMINFO_XFERS_PENDING`.

        curl_off_t
        get_xfers_pending()
            const;

        std::expected<curl_off_t, error>
        try_get_xfers_pending()
            const noexcept;

        /// Wrapper for `CURLMINFO_XFERS_DONE`.

        curl_off_t
        get_xfers_done()
            const;

        std::expected<curl_off_t, error>
        try_get_xfers_done()
            const noexcept;

        /// Wrapper for `CURLMINFO_XFERS_ADDED`.

        curl_off_t
        get_xfers_added()
            const;

        std::expected<curl_off_t, error>
        try_get_xfers_added()
            const noexcept;

        // End of getters

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


    private:

        void
        setup_callbacks();


        // Callback helpers

#if CURL_AT_LEAST_VERSION(8, 17, 0)

        static
        void
        notify_helper(CURLM* raw_multi,
                      unsigned type,
                      CURL* raw_handle,
                      void* ctx)
            noexcept;

#endif

        static
        int
        push_helper(CURL* raw_parent_handle,
                    CURL* raw_new_handle,
                    std::size_t num_headers,
                    curl_pushheaders* headers,
                    void* ctx);

        static
        int
        socket_helper(CURL* raw_handle,
                      curl_socket_t fd,
                      int what,
                      void* ctx,
                      void* socket_data)
            noexcept;

        static
        int
        timer_helper(CURLM* handle,
                     long timeout_ms,
                     void* ctx)
            noexcept;


        // Private data

        extra_state_type extra_state;

    }; // struct multi

} // namespace curl

#endif
