/*
 * basic_wrapper - base class for C++ wrappers
 * Copyright 2025-2026  Daniel K. O. (dkosmari)
 *
 * This software can be distributed under the terms of any of the following licenses:
 *   SPDX-License-Identifier: Apache-2.0
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 *   SPDX-License-Identifier: MIT
 *   SPDX-License-Identifier: Zlib
 *
 * Source: https://github.com/dkosmari/basic_wrapper
 */

#ifndef CURLXX_DETAIL_OWNER_WRAPPER_HPP
#define CURLXX_DETAIL_OWNER_WRAPPER_HPP

#include <tuple>

#include "basic_wrapper.hpp"


namespace curl::detail {

    // This wrapper has an "owner" flag. When false, it will not destroy the raw object.
    template<typename T,
             T InvalidValue = T{}>
    class owner_wrapper : public basic_wrapper<T, InvalidValue> {

    protected:

        bool owner = true;


    public:

        using base_type = basic_wrapper<T, InvalidValue>;

        using state_type = std::tuple<typename base_type::state_type, bool>;


        // Inherit constructors.
        using base_type::base_type;


        /// Move constructor.
        owner_wrapper(owner_wrapper&& other)
            noexcept
        {
            acquire(other.release());
        }


        /// Move assignment.
        owner_wrapper&
        operator =(owner_wrapper&& other)
            noexcept
        {
            if (this != &other) {
                this->destroy();
                acquire(other.release());
            }
            return *this;
        }


        void
        acquire(state_type state)
            noexcept
        {
            base_type::acquire(std::move(get<0>(state)));
            owner = get<1>(state);
        }


        void
        acquire(typename base_type::state_type new_parent_state,
                bool new_owner = true)
            noexcept
        {
            acquire(state_type{new_parent_state, new_owner});
        }


        state_type
        release()
            noexcept
        {
            bool old_owner = owner;
            owner = false;
            return state_type{
                base_type::release(),
                old_owner
            };
        }


        // Turn into an observer.
        void
        disown()
            noexcept
        {
            owner = false;
        }


    }; // class owner_wrapper

} // namespace curl::detail

#endif
