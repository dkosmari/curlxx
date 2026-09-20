/*
 * curlxx - A C++ wrapper for libcurl.
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <cassert>
#include <new>

#include "curlxx/multi.hpp"

#include "curlxx/easy.hpp"
#include "utils.hpp"


using std::expected;
using std::unexpected;

using curl::utils::value_or_throw;


namespace curl {

    namespace {

        /*-----------------------*/
        /* Function declarations */
        /*-----------------------*/

#if CURL_AT_LEAST_VERSION(8, 16, 0)

        std::expected<curl_off_t, error>
        wrap_get_offt(CURLM* raw,
                      CURLMinfo info);

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)

        template<typename T>
        std::expected<void, error>
        wrap_setopt(CURLM* raw,
                    CURLMoption opt,
                    T arg)
            noexcept;

        void
        wrap_unsetopt(CURLM* raw,
                      CURLMoption opt)
            noexcept;


        /*----------------------*/
        /* Function definitions */
        /*----------------------*/

#if CURL_AT_LEAST_VERSION(8, 16, 0)

        std::expected<curl_off_t, error>
        wrap_get_offt(CURLM* raw,
                      CURLMinfo info)
        {
            curl_off_t result;
            auto e = curl_multi_get_offt(raw, info, &result);
            if (e)
                return std::unexpected{error{e}};
            return result;
        }

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


        template<typename T>
        std::expected<void, error>
        wrap_setopt(CURLM* raw,
                    CURLMoption opt,
                    T arg)
            noexcept
        {
            auto e = curl_multi_setopt(raw, opt, arg);
            if (e)
                return std::unexpected{error{e}};
            return {};
        }


        void
        wrap_unsetopt(CURLM* raw,
                      CURLMoption opt)
            noexcept
        {
            curl_multi_setopt(raw, opt, static_cast<void*>(nullptr));
        }

    } // namespace


    /*------------------*/
    /* Public functions */
    /*------------------*/

    pushheaders::pushheaders(curl_pushheaders* raw_,
                             std::size_t size_)
        noexcept :
        raw{raw_},
        num_headers{size_}
    {}


    curl_pushheaders*
    pushheaders::data()
        const noexcept
    {
        return raw;
    }


    bool
    pushheaders::empty()
        const noexcept
    {
        return num_headers == 0;
    }


    std::size_t
    pushheaders::size()
        const noexcept
    {
        return num_headers;
    }


    std::optional<std::string_view>
    pushheaders::operator [](const char* name)
        const noexcept
    {
        auto result = curl_pushheader_byname(raw, name);
        if (!result)
            return {};
        return std::string_view{result};
    }


    std::optional<std::string_view>
    pushheaders::operator [](const std::string& name)
        const noexcept
    {
        auto result =  curl_pushheader_byname(raw, name.data());
        if (!result)
            return {};
        return std::string_view{result};
    }


    std::optional<std::string_view>
    pushheaders::operator [](std::size_t num)
        const noexcept
    {
        auto result = curl_pushheader_bynum(raw, num);
        if (!result)
            return {};
        return std::string_view{result};
    }


    multi::multi()
    {
        create();
    }


    multi::multi(CURLM* handle)
    {
        create(handle);
    }


    multi::~multi()
        noexcept
    {
        destroy();
    }


    void
    multi::create()
    {
        auto new_raw = curl_multi_init();
        if (!new_raw)
            throw error{"curl_multi_init() failed"};
        destroy();
        acquire(new_raw);
    }


    void
    multi::create(CURLM* handle)
    {
        destroy();
        acquire(handle);
    }


    void
    multi::destroy()
        noexcept
    {
        if (is_valid()) {
            auto [old_raw, old_state] = release();
            curl_multi_cleanup(old_raw);
        }
    }


    void
    multi::acquire(state_type new_state)
        noexcept
    {
        base_type::acquire(get<0>(new_state));
        extra_state = std::move(get<1>(new_state));
        setup_callbacks();
    }


    void
    multi::acquire(raw_type new_raw)
        noexcept
    {
        base_type::acquire(new_raw);
        setup_callbacks();
    }


    multi::state_type
    multi::release()
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
    multi::add(easy& ez)
    {
        return value_or_throw(try_add(ez));
    }


    expected<void, error>
    multi::try_add(easy& ez)
        noexcept
    {
        auto e = curl_multi_add_handle(raw, ez.data());
        if (e)
            return unexpected{error{e}};
        return {};
    }


    void
    multi::remove(easy& ez)
    {
        return value_or_throw(try_remove(ez));
    }


    expected<void, error>
    multi::try_remove(easy& ez)
        noexcept
    {
        auto e = curl_multi_remove_handle(raw, ez.data());
        if (e)
            return unexpected{error{e}};
        return {};
    }


    unsigned
    multi::perform()
    {
        return value_or_throw(try_perform());
    }


    expected<unsigned, error>
    multi::try_perform()
        noexcept
    {
        int running_handles = 0;
        auto e = curl_multi_perform(raw, &running_handles);
        if (e)
            return unexpected{error{e}};
        return running_handles;
    }


    std::optional<multi::msg_done>
    multi::info_read()
        noexcept
    {
        int pending;
        return info_read(pending);
    }


    std::optional<multi::msg_done>
    multi::info_read(int& pending)
        noexcept
    {
        if (auto info = curl_multi_info_read(raw, &pending)) {
            // Assume only "CURLMSG_DONE" messages exist.
            if (info->msg == CURLMSG_DONE) {
                return msg_done{
                    .raw_handle = info->easy_handle,
                    .handle = easy::get_wrapper(info->easy_handle),
                    .result = info->data.result,
                };
            }
        }
        return {};
    }


    std::vector<multi::msg_done>
    multi::get_done()
    {
        std::vector<msg_done> result;
        int pending;
        while (auto info = curl_multi_info_read(raw, &pending)) {
            // Assume only "CURLMSG_DONE" messages exist.
            if (info->msg == CURLMSG_DONE)
                result.emplace_back(info->easy_handle,
                                    easy::get_wrapper(info->easy_handle),
                                    info->data.result);
        }
        return result;
    }


    int
    multi::poll(std::span<curl_waitfd> extra_fds,
                std::chrono::milliseconds timeout)
    {
        return value_or_throw(try_poll(extra_fds, timeout));
    }


    int
    multi::poll(std::span<curl_waitfd> extra_fds,
                std::chrono::milliseconds timeout,
                std::stop_token stopper)
    {
        return value_or_throw(try_poll(extra_fds, timeout, std::move(stopper)));
    }


    int
    multi::poll(std::chrono::milliseconds timeout)
    {
        return value_or_throw(try_poll(timeout));
    }


    int
    multi::poll(std::chrono::milliseconds timeout,
                std::stop_token stopper)
    {
        return value_or_throw(try_poll(timeout, std::move(stopper)));
    }


    std::expected<int, error>
    multi::try_poll(std::span<curl_waitfd> extra_fds,
                    std::chrono::milliseconds timeout)
        noexcept
    {
        int timeout_ms = timeout.count();
        int result;
        auto e = curl_multi_poll(raw,
                                 extra_fds.data(),
                                 extra_fds.size(),
                                 timeout_ms,
                                 &result);
        if (e)
            return std::unexpected{error{e}};
        return result;
    }


    std::expected<int, error>
    multi::try_poll(std::span<curl_waitfd> extra_fds,
                    std::chrono::milliseconds timeout,
                    std::stop_token stopper)
        noexcept
    {
        int timeout_ms = timeout.count();
        int result;
        std::stop_callback stop_on_wakeup{
            stopper,
            [this] { wakeup(); }
        };
        auto e = curl_multi_poll(raw,
                                 extra_fds.data(),
                                 extra_fds.size(),
                                 timeout_ms,
                                 &result);
        if (e)
            return std::unexpected{error{e}};
        return result;
    }


    std::expected<int, error>
    multi::try_poll(std::chrono::milliseconds timeout)
        noexcept
    {
        return try_poll({}, timeout);
    }


    std::expected<int, error>
    multi::try_poll(std::chrono::milliseconds timeout,
                    std::stop_token stopper)
        noexcept
    {
        return try_poll({}, timeout, std::move(stopper));
    }


    void
    multi::wakeup()
    {
        return value_or_throw(try_wakeup());
    }


    std::expected<void, error>
    multi::try_wakeup()
        noexcept
    {
        auto e = curl_multi_wakeup(raw);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    int
    multi::wait(std::span<curl_waitfd> extra_fds,
                std::chrono::milliseconds timeout)
    {
        return value_or_throw(try_wait(extra_fds, timeout));
    }


    std::expected<int, error>
    multi::try_wait(std::span<curl_waitfd> extra_fds,
                    std::chrono::milliseconds timeout)
        noexcept
    {
        int timeout_ms = timeout.count();
        int result;
        auto e = curl_multi_wait(raw,
                                 extra_fds.data(),
                                 extra_fds.size(),
                                 timeout_ms,
                                 &result);
        if (e)
            return std::unexpected{error{e}};
        return result;
    }


#if CURL_AT_LEAST_VERSION(8, 8, 0)

    unsigned
    multi::waitfds(std::span<curl_waitfd> ufds)
    {
        return value_or_throw(try_waitfds(ufds));
    }


    std::expected<unsigned, error>
    multi::try_waitfds(std::span<curl_waitfd> ufds)
        noexcept
    {
        unsigned result;
        auto e = curl_multi_waitfds(raw,
                                    ufds.data(),
                                    ufds.size(),
                                    &result);
        if (e)
            return std::unexpected{error{e}};
        return result;
    }

#endif // CURL_AT_LEAST_VERSION(8, 8, 0)


    void
    multi::fdset(fd_set& read_set,
                 fd_set& write_set,
                 fd_set& except_set,
                 int& max_fd)
        const
    {
        return value_or_throw(try_fdset(read_set, write_set, except_set, max_fd));
    }


    std::expected<void, error>
    multi::try_fdset(fd_set& read_set,
                  fd_set& write_set,
                  fd_set& except_set,
                  int& max_fd)
        const noexcept
    {
        auto e = curl_multi_fdset(raw, &read_set, &write_set, &except_set, &max_fd);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


#if CURL_AT_LEAST_VERSION(8, 4, 0)

    std::vector<easy*>
    multi::get_handles()
        const
    {
        return value_or_throw(try_get_handles());
    }


    std::expected<std::vector<easy*>, error>
    multi::try_get_handles()
        const noexcept
    {
        auto raw_handles = curl_multi_get_handles(raw);
        if (!raw_handles)
            return std::unexpected{error{CURLM_OUT_OF_MEMORY}};
        try {
            std::vector<easy*> result;
            for (CURL* raw_handle = raw_handles[0]; raw_handle; ++raw_handle)
                if (auto ez = easy::get_wrapper(raw_handle))
                    result.push_back(ez);

            curl_free(raw_handles);
            raw_handles = nullptr;

            return {std::move(result)};
        }
        catch (std::bad_alloc&) {
            curl_free(raw_handles);
            return std::unexpected{error{CURLM_OUT_OF_MEMORY}};
        }
        catch (...) {
            curl_free(raw_handles);
            return std::unexpected{error{CURLM_INTERNAL_ERROR}};
        }
    }

#endif // CURL_AT_LEAST_VERSION(8, 4, 0)


#if CURL_AT_LEAST_VERSION(8, 17, 0)

    void
    multi::notify_disable(unsigned type)
    {
        return value_or_throw(try_notify_disable(type));
    }


    std::expected<void, error>
    multi::try_notify_disable(unsigned type)
        noexcept
    {
        auto e = curl_multi_notify_disable(raw, type);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    void
    multi::notify_enable(unsigned type)
    {
        return value_or_throw(curl_multi_notify_enable(raw, type));
    }


    std::expected<void, error>
    multi::try_notify_enable(unsigned type)
        noexcept
    {
        auto e = curl_multi_notify_enable(raw, type);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }

#endif // CURL_AT_LEAST_VERSION(8, 17, 0)


    void
    multi::assign(curl_socket_t fd,
                  void* data)
    {
        return value_or_throw(try_assign(fd, data));
    }


    std::expected<void, error>
    multi::try_assign(curl_socket_t fd,
                      void* data)
        noexcept
    {
        auto e = curl_multi_assign(raw, fd, data);
        if (e)
            return std::unexpected{error{e}};
        return {};
    }


    int
    multi::socket_action(curl_socket_t fd,
                         int event)
    {
        return value_or_throw(try_socket_action(fd, event));
    }


    std::expected<int, error>
    multi::try_socket_action(curl_socket_t fd,
                             int event)
        noexcept
    {
        int result;
        auto e = curl_multi_socket_action(raw, fd, event, &result);
        if (e)
            return std::unexpected{error{e}};
        return result;
    }


    std::chrono::milliseconds
    multi::get_timeout()
    {
        return value_or_throw(try_get_timeout());
    }


    std::expected<std::chrono::milliseconds, error>
    multi::try_get_timeout()
        noexcept
    {
        long timeout_ms;
        auto e = curl_multi_timeout(raw, &timeout_ms);
        if (e)
            return std::unexpected{error{e}};
        return std::chrono::milliseconds(timeout_ms);
    }


    void
    multi::set_max_connections(long n)
    {
        return value_or_throw(try_set_max_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_connections(long n)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_MAXCONNECTS, n);
    }


    void
    multi::set_max_concurrent_streams(long n)
    {
        return value_or_throw(try_set_max_concurrent_streams(n));
    }


    std::expected<void, error>
    multi::try_set_max_concurrent_streams(long n)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_MAX_CONCURRENT_STREAMS, n);
    }


    void
    multi::set_max_host_connections(long n)
    {
        return value_or_throw(try_set_max_host_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_host_connections(long n)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_MAX_HOST_CONNECTIONS, n);
    }


    void
    multi::set_max_total_connections(long n)
    {
        return value_or_throw(try_set_max_total_connections(n));
    }


    std::expected<void, error>
    multi::try_set_max_total_connections(long n)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_MAX_TOTAL_CONNECTIONS, n);
    }


#if CURL_AT_LEAST_VERSION(8, 16, 0)

    void
    multi::set_network_changed(long mask)
    {
        return value_or_throw(try_set_network_changed(mask));
    }


    std::expected<void, error>
    multi::try_set_network_changed(long mask)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_NETWORK_CHANGED, mask);
    }

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


#if CURL_AT_LEAST_VERSION(8, 17, 0)

    void
    multi::set_notify_function(notify_function_t func)
        noexcept
    {
        extra_state.notify_func = std::move(func);
        setup_callbacks();
    }


    void
    multi::unset_notify_function()
        noexcept
    {
        extra_state.notify_func = {};
        setup_callbacks();
    }

#endif // CURL_AT_LEAST_VERSION(8, 17, 0)


    void
    multi::set_pipelining(long mask)
    {
        return value_or_throw(try_set_pipelining(mask));
    }


    std::expected<void, error>
    multi::try_set_pipelining(long mask)
        noexcept
    {
        return wrap_setopt(raw, CURLMOPT_PIPELINING, mask);
    }


    void
    multi::set_push_function(push_function_t func)
        noexcept
    {
        extra_state.push_func = std::move(func);
        setup_callbacks();
    }


    void
    multi::unset_push_function()
        noexcept
    {
        extra_state.push_func = {};
        setup_callbacks();
    }


    void
    multi::set_socket_function(socket_function_t func)
        noexcept
    {
        extra_state.socket_func = std::move(func);
        setup_callbacks();
    }


    void
    multi::unset_socket_function()
        noexcept
    {
        extra_state.socket_func = {};
        setup_callbacks();
    }


    void
    multi::set_timer_function(timer_function_t func)
        noexcept
    {
        extra_state.timer_func = std::move(func);
        setup_callbacks();
    }


    void
    multi::unset_timer_function()
        noexcept
    {
        extra_state.timer_func = {};
        setup_callbacks();
    }


#if CURL_AT_LEAST_VERSION(8, 16, 0)

    curl_off_t
    multi::get_xfers_current()
        const
    {
        return value_or_throw(try_get_xfers_current());
    }


    std::expected<curl_off_t, error>
    multi::try_get_xfers_current()
        const noexcept
    {
        return wrap_get_offt(raw, CURLMINFO_XFERS_CURRENT);
    }


    curl_off_t
    multi::get_xfers_running()
        const
    {
        return value_or_throw(try_get_xfers_running());
    }


    std::expected<curl_off_t, error>
    multi::try_get_xfers_running()
        const noexcept
    {
        return wrap_get_offt(raw, CURLMINFO_XFERS_RUNNING);
    }


    curl_off_t
    multi::get_xfers_pending()
        const
    {
        return value_or_throw(try_get_xfers_pending());
    }


    std::expected<curl_off_t, error>
    multi::try_get_xfers_pending()
        const noexcept
    {
        return wrap_get_offt(raw, CURLMINFO_XFERS_PENDING);
    }


    curl_off_t
    multi::get_xfers_done()
        const
    {
        return value_or_throw(try_get_xfers_done());
    }


    std::expected<curl_off_t, error>
    multi::try_get_xfers_done()
        const noexcept
    {
        return wrap_get_offt(raw, CURLMINFO_XFERS_DONE);
    }


    curl_off_t
    multi::get_xfers_added()
        const
    {
        return value_or_throw(try_get_xfers_added());
    }


    std::expected<curl_off_t, error>
    multi::try_get_xfers_added()
        const noexcept
    {
        return wrap_get_offt(raw, CURLMINFO_XFERS_ADDED);
    }

#endif // CURL_AT_LEAST_VERSION(8, 16, 0)


    void
    multi::setup_callbacks()
    {
#if CURL_AT_LEAST_VERSION(8, 17, 0)
        // Update notify function.
        if (extra_state.notify_func) {
            curl_multi_setopt(raw, CURLMOPT_NOTIFYDATA, this);
            curl_multi_setopt(raw, CURLMOPT_NOTIFYFUNCTION, &multi::notify_helper);
        } else {
            wrap_unsetopt(raw, CURLMOPT_NOTIFYDATA);
            wrap_unsetopt(raw, CURLMOPT_NOTIFYFUNCTION);
        }
#endif // CURL_AT_LEAST_VERSION(8, 17, 0)

        // Update push function.
        if (extra_state.push_func) {
            curl_multi_setopt(raw, CURLMOPT_PUSHDATA, this);
            curl_multi_setopt(raw, CURLMOPT_PUSHFUNCTION, &multi::push_helper);
        } else {
            wrap_unsetopt(raw, CURLMOPT_SOCKETDATA);
            wrap_unsetopt(raw, CURLMOPT_SOCKETFUNCTION);
        }

        // Update socket function.
        if (extra_state.socket_func) {
            curl_multi_setopt(raw, CURLMOPT_SOCKETDATA, this);
            curl_multi_setopt(raw, CURLMOPT_SOCKETFUNCTION, &multi::socket_helper);
        } else {
            wrap_unsetopt(raw, CURLMOPT_SOCKETDATA);
            wrap_unsetopt(raw, CURLMOPT_SOCKETFUNCTION);
        }

        // Update timer function.
        if (extra_state.timer_func) {
            curl_multi_setopt(raw, CURLMOPT_TIMERDATA, this);
            curl_multi_setopt(raw, CURLMOPT_TIMERFUNCTION, &multi::timer_helper);
        } else {
            wrap_unsetopt(raw, CURLMOPT_TIMERDATA);
            wrap_unsetopt(raw, CURLMOPT_TIMERFUNCTION);
        }
    }


#if CURL_AT_LEAST_VERSION(8, 17, 0)

    void
    multi::notify_helper(CURLM* raw_multi,
                         unsigned type,
                         CURL* raw_handle,
                         void* ctx)
        noexcept
    {
        auto self = reinterpret_cast<multi*>(ctx);
        assert(self);
        assert(raw_multi == self.raw);
        if (self->extra_state.notify_func) {
            auto handle = easy::get_wrapper(raw_handle);
            self->extra_state.notify_func(type, handle, raw_handle);
        }
    }

#endif // CURL_AT_LEAST_VERSION(8, 17, 0)


    int
    multi::push_helper(CURL* raw_parent_handle,
                       CURL* raw_new_handle,
                       std::size_t num_headers,
                       curl_pushheaders* headers,
                       void* ctx)
    {
        auto self = reinterpret_cast<multi*>(ctx);
        assert(self);
        try {
            if (self->extra_state.push_func) {
                auto parent_handle = easy::get_wrapper(raw_parent_handle);
                return self->extra_state.push_func(parent_handle,
                                                   raw_parent_handle,
                                                   raw_new_handle,
                                                   pushheaders{headers, num_headers});
            } else
                return CURL_PUSH_DENY;
        }
        catch (...) {
            return CURL_PUSH_ERROROUT;
        }
    }


    int
    multi::socket_helper(CURL* raw_handle,
                         curl_socket_t fd,
                         int what,
                         void* ctx,
                         void* socket_data)
        noexcept
    {
        auto self = reinterpret_cast<multi*>(ctx);
        assert(self);
        if (self->extra_state.socket_func) {
            auto handle = easy::get_wrapper(raw_handle);
            try {
                return self->extra_state.socket_func(
                    handle,
                    raw_handle,
                    fd,
                    what,
                    socket_data
                );
            }
            catch (...) {
                return -1; // signal error
            }
        } else
            return 0;
    }


    int
    multi::timer_helper(CURLM*,
                        long timeout_ms,
                        void* ctx)
        noexcept
    {
        auto self = reinterpret_cast<multi*>(ctx);
        assert(self);
        if (self->extra_state.timer_func) {
            try {
                return self->extra_state.timer_func(std::chrono::milliseconds(timeout_ms));
            }
            catch (...) {
                return -1; // signal error
            }
        } else {
            return -1; // signal error
        }
    }

} // namespace curl
