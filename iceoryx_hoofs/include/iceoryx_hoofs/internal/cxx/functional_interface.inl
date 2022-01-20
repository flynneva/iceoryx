// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_HOOFS_CXX_FUNCTIONAL_INTERFACE_INL
#define IOX_HOOFS_CXX_FUNCTIONAL_INTERFACE_INL

namespace iox
{
namespace cxx
{
namespace internal
{
template <typename T>
inline void Expect<T>::expect(const char* const msg) const noexcept
{
    if (!(*static_cast<const T*>(this)))
    {
        std::cout << msg << std::endl;
        Ensures(false);
    }
}

template <typename T>
inline typename ExpectWithValue<T>::expect_value_t& ExpectWithValue<T>::expect(const char* const msg) & noexcept
{
    T* upcastedThis = static_cast<T*>(this);

    if (!(*upcastedThis))
    {
        std::cout << msg << std::endl;
        Ensures(false);
    }

    return *upcastedThis;
}

template <typename T>
inline const typename ExpectWithValue<T>::expect_value_t&
ExpectWithValue<T>::expect(const char* const msg) const& noexcept
{
    return const_cast<const T&>(const_cast<Expect<T>*>(this)->expect(msg));
}

template <typename T>
inline typename ExpectWithValue<T>::expect_value_t&& ExpectWithValue<T>::expect(const char* const msg) && noexcept
{
    return std::move(this->expect(msg));
}

template <typename T>
inline const typename ExpectWithValue<T>::expect_value_t&&
ExpectWithValue<T>::expect(const char* const msg) const&& noexcept
{
    return const_cast<const T&&>(std::move(const_cast<Expect<T>*>(this)->expect(msg)));
}

template <typename T>
inline T& AndThenWithValue<T>::and_then(const and_then_callback_t& callable) & noexcept
{
    T* upcastedThis = static_cast<T*>(this);

    if (*upcastedThis)
    {
        callable(upcastedThis->value());
    }

    return *upcastedThis;
}

template <typename T>
inline const T& AndThenWithValue<T>::and_then(const and_then_callback_t& callable) const& noexcept
{
    return const_cast<const T&>(const_cast<AndThenWithValue<T>*>(this)->and_then(callable));
}

} // namespace internal
} // namespace cxx
} // namespace iox
#endif
