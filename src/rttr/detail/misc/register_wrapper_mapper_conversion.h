/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 - 2017 Axel Menzel <info@rttr.org>                     *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_REGISTER_WRAPPER_MAPPER_CONVERSION_H_
#define RTTR_REGISTER_WRAPPER_MAPPER_CONVERSION_H_

#include "rttr/detail/base/core_prerequisites.h"

#include "rttr/detail/misc/misc_type_traits.h"

#include <type_traits>

namespace rttr
{
template<typename... U> struct type_list;

namespace detail
{

/*!
 * Determine if the given type \a T has a wrapper_mapper method called `convert`.
 */
template <typename T>
class has_conversion_function_impl
{
    typedef char YesType[1];
    typedef char NoType[2];

    template <typename U> static YesType& check(decltype(&wrapper_mapper<U>::convert));
    template <typename U> static NoType&  check(...);
public:
    static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(check<raw_type_t<T>>(0)) != sizeof(YesType));
};

/*!
 * If \a T has a wrapper_mapper function `convert` then is the same like `std::true_type`, otherwise inherits from `std::false_type`.
 */
template<typename T>
using has_wrapper_conv_func = std::integral_constant<bool, has_conversion_function_impl<T>::value>;

/////////////////////////////////////////////////////////////////////////////////////


template<typename Derived, typename Base>
struct create_wrapper_conversion
{
    static RTTR_INLINE void perform()
    {
        using base_wrapped_type = raw_type_t<wrapper_mapper_t<Base>>;
        type::register_converter_func(wrapper_mapper<Derived>::template convert<base_wrapped_type>);
        using return_type = typename function_traits<decltype(&wrapper_mapper<Derived>::template convert<base_wrapped_type>)>::return_type;
        // TO DO: remove raw_type_t, std::shared_ptr<const T> should also be converted, when necessary
        using wrapped_derived_t = raw_type_t<wrapper_mapper_t<Derived>>;
        type::register_converter_func(wrapper_mapper<return_type>::template convert<wrapped_derived_t>);

    }
};


/////////////////////////////////////////////////////////////////////////////////////


template<typename Derived, typename Base>
struct reg_wrapper_converter_for_base_classes
{
    static_assert(is_wrapper<Derived>::value && is_wrapper<Base>::value);
    reg_wrapper_converter_for_base_classes()
    {
        create_wrapper_conversion<Derived, Base>::perform();
    }
};

/////////////////////////////////////////////////////////////////////////////////////

}   // end namespace detail
}   // end namespace rttr

#endif // RTTR_REGISTER_WRAPPER_MAPPER_CONVERSION_H_
