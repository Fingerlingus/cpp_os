#pragma once

#include <type_traits>

#include "utility.hpp"

namespace kstd {

template<typename Derived, typename Base> concept derived_from =
  std::is_base_of_v<Base, Derived> &&
  std::is_convertible_v<const volatile Derived*, const volatile Base*>;

template <typename T> concept destructible = std::is_nothrow_destructible_v<T>;

template <typename T, typename ...Args> concept constructible_from =
    destructible<T> && 
    std::is_constructible_v<T, Args...>;

template <class From, class To> concept convertible_to =
    std::is_convertible_v<From, To> &&
    requires {
        static_cast<To>(std::declval<From>());
    };

template<typename T> concept move_constructible = 
    constructible_from<T, T> && 
    convertible_to<T, T>;

template <typename T> concept copy_constructible = 
    move_constructible<T> &&
    constructible_from<T, T&> && convertible_to<T&, T> &&
    constructible_from<T, const T&> && convertible_to<const T&, T> &&
    constructible_from<T, const T> && convertible_to<const T, T>;

namespace detail {

template<typename T, typename U> 
concept same_as_impl = std::is_same_v<T, U>;

} // namespace detail
 
template<typename T, typename U>
concept same_as = detail::same_as_impl<T, U> && detail::same_as_impl<U, T>;

template <typename T, typename U> concept common_reference_with =
  same_as<std::common_reference_t<T, U>, std::common_reference_t<U, T>> &&
  convertible_to<T, std::common_reference_t<T, U>> &&
  convertible_to<U, std::common_reference_t<T, U>>;

template<typename Lhs, typename Rhs> concept assignable_from =
    std::is_lvalue_reference_v<Lhs> &&
    common_reference_with<
        const std::remove_reference_t<Lhs>&,
        const std::remove_reference_t<Rhs>&> &&
    requires(Lhs lhs, Rhs&& rhs) {
        { lhs = forward<Rhs>(rhs) } -> same_as<Lhs>;
    };

template <typename T> concept movable =
    std::is_object_v<T> &&
    move_constructible<T> &&
    assignable_from<T&, T> /*&&
    std::swappable<T>*/;

template <typename T> concept copyable =
    copy_constructible<T> &&
    movable<T> &&
    assignable_from<T&, T&> &&
    assignable_from<T&, const T&> &&
    assignable_from<T&, const T>;

template<typename T> concept default_initializable =
    constructible_from<T> &&
    requires { T{}; } /*&&
    requires { ::new (static_cast<void*>(nullptr)) T; }*/;

namespace detail {

template<class B> concept boolean_testable_impl = convertible_to<B, bool>;

} // namespace detail
  
template<class B> concept boolean_testable =  
    detail::boolean_testable_impl<B> &&
    requires (B&& b) {
        { !forward<B>(b) } -> detail::boolean_testable_impl;
    };

template<class T, class U> concept __WeaklyEqualityComparableWith 
    = requires(const std::remove_reference_t<T>& t,
               const std::remove_reference_t<U>& u) 
    {
        { t == u } -> boolean_testable;
        { t != u } -> boolean_testable;
        { u == t } -> boolean_testable;
        { u != t } -> boolean_testable;
    };

template <typename T>
concept equality_comparable = __WeaklyEqualityComparableWith<T, T>;

template <class T, class U> concept equality_comparable_with =
    equality_comparable<T> &&
    equality_comparable<U> &&
    common_reference_with<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&> &&
    equality_comparable<
        std::common_reference_t<
            const std::remove_reference_t<T>&,
            const std::remove_reference_t<U>&>> &&
  __WeaklyEqualityComparableWith<T, U>;



template <typename T>
concept semiregular = copyable<T> && default_initializable<T>;

template <typename T> concept regular = semiregular<T> && 
                                        equality_comparable<T>;
} // namespace kstd
