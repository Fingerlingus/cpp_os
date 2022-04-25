#pragma once

#include <cstdint>

#include "concept.hpp"
#include "utility.hpp"

namespace nonstd {

struct base_iterator_tag { };
struct input_iterator_tag { };
struct output_iterator_tag {  };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };
struct contiguous_iterator_tag : public random_access_iterator_tag { };

template <typename Category, 
          typename T, 
          typename Distance = std::intmax_t, 
          typename Pointer = T*, 
          typename Reference = T&> 
struct iterator {
	using value_type = T;
	using difference_type = Distance;
	using pointer = Pointer;
	using reference = Reference;
	using iterator_category = Category;
};

namespace detail {

template<typename Iter> concept iter_concept = requires {
    typename Iter::difference_type;
    typename Iter::value_type;
    typename Iter::pointer;
    typename Iter::reference;
    typename Iter::iterator_category;
}; 

} // namespace detail

template<typename Iter>
    requires detail::iter_concept<Iter>
struct iterator_traits {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template<class Iter> concept weakly_incrementable =
    movable<Iter> &&
    requires(Iter i) {
        { ++i } -> same_as<Iter&>;   // not required to be equality-preserving
        i++;                         // not required to be equality-preserving
    };



template<class Iter> concept incrementable =
    regular<Iter> &&
    weakly_incrementable<Iter> /*&&
    requires(Iter i) {
        { i++ } -> same_as<Iter>;
    }*/;

namespace detail {

template<typename T> concept dereferenceable = !same_as<T, void>;

} // namespace detail
  
template<typename T>
    requires detail::dereferenceable<T>
using iter_value_t = 
    typename iterator_traits<std::remove_cvref_t<T>>::value_type;

template<typename T> 
    requires detail::dereferenceable<T>
using iter_reference_t = decltype(*declval<T&>());

template<typename T> using iter_difference_t = 
    typename iterator_traits<std::remove_cvref_t<T>>::difference_type;

template<typename T>
    requires detail::dereferenceable<T>
using iter_rvalue_reference_t = decltype(std::declval<T&&>());

template<typename Iter> concept __IndirectlyReadableImpl = // exposition only
    requires(const Iter it) {
      typename iter_value_t<Iter>;
      typename iter_reference_t<Iter>;
      typename iter_rvalue_reference_t<Iter>;
      //{ *it } -> same_as<iter_reference_t<Iter>>;
    /* TODO - implement ranges
      { ranges::iter_move(in) } -> std::same_as<std::iter_rvalue_reference_t<In>>;
    */
    }                                                          &&
    common_reference_with<iter_reference_t<Iter>&&, 
                          iter_value_t<Iter>&>                 &&
    common_reference_with<iter_reference_t<Iter>&&, 
                          iter_rvalue_reference_t<Iter>&&>     &&
    common_reference_with<iter_rvalue_reference_t<Iter>&&, 
                          const iter_value_t<Iter>&>;

template<typename Iter> concept indirectly_readable =
    __IndirectlyReadableImpl<std::remove_cvref_t<Iter>>;

template<indirectly_readable T>
using iter_common_reference_t = 
    typename std::common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;

namespace detail {

template<typename T> concept can_reference = !same_as<T, void>;

} // namespace detail

template <class Iter> concept input_or_output_iterator =
    detail::iter_concept<Iter> &&
    weakly_incrementable<Iter>;


template<class Iter> concept input_iterator =
    input_or_output_iterator<Iter> &&
    indirectly_readable<Iter> /*&&
    derived_from<Iter, input_iterator_tag>*/;

template<typename S, typename I> concept sentinel_for =
    semiregular<S> &&
    input_or_output_iterator<I> &&
    __WeaklyEqualityComparableWith<S, I>;

template<class S, class I> 
inline constexpr bool disable_sized_sentinel_for = false;

template<typename S, typename I> concept sized_sentinel_for =
    sentinel_for<S, I> &&
    !disable_sized_sentinel_for<std::remove_cv_t<S>, std::remove_cv_t<I>> &&
    requires(const I& i, const S& s) {
        { s - i } -> same_as<iter_difference_t<I>>;
        { i - s } -> same_as<iter_difference_t<I>>;
    };

template<typename Iter> concept forward_iterator =
    input_iterator<Iter> &&
    //derived_from<Iter, forward_iterator_tag> &&
    incrementable<Iter> &&
    sentinel_for<Iter, Iter>;

template<typename Iter> concept bidirectional_iterator = 
    forward_iterator<Iter> &&
    //derived_from<Iter, bidirectional_iterator_tag> &&
    requires(Iter i) {
        { --i } -> same_as<Iter&>;
        { i-- } -> same_as<Iter>;
    };

template<class T, class U> concept __PartiallyOrderedWith =
requires(const std::remove_reference_t<T>& t,
         const std::remove_reference_t<U>& u) 
    {
        { t <  u } -> boolean_testable;
        { t >  u } -> boolean_testable;
        { t <= u } -> boolean_testable;
        { t >= u } -> boolean_testable;
        { u <  t } -> boolean_testable;
        { u >  t } -> boolean_testable;
        { u <= t } -> boolean_testable;
        { u >= t } -> boolean_testable;
  };

template<class T> concept totally_ordered = equality_comparable<T> &&
                                            __PartiallyOrderedWith<T, T>;

template<typename T, typename U> concept totally_ordered_with =
    totally_ordered<T> &&
    totally_ordered<U> &&
    equality_comparable_with<T, U> &&
    totally_ordered<
        std::common_reference_t<
            const std::remove_reference_t<T>&,
            const std::remove_reference_t<U>&>> &&
    __PartiallyOrderedWith<T, U>;

template<typename Iter> concept random_access_iterator =
    bidirectional_iterator<Iter> &&
    derived_from<Iter, random_access_iterator_tag> &&
    totally_ordered<Iter> &&
    sized_sentinel_for<Iter, Iter> &&
    requires(Iter i, const Iter j, const iter_difference_t<Iter> n) {
        { i += n } -> same_as<Iter&>;
        { j +  n } -> same_as<Iter>;
        { n +  j } -> same_as<Iter>;
        { i -= n } -> same_as<Iter&>;
        { j -  n } -> same_as<Iter>;
        { j[n]   } -> same_as<iter_reference_t<Iter>>;
    };

template<typename Iter>
//    requires bidirectional_iterator<Iter>
class reverse_iterator {
public:
    using value_type = Iter;
    using self_type = reverse_iterator<Iter>;

    constexpr explicit reverse_iterator(const value_type& it): m_it(it) { }
    constexpr explicit reverse_iterator(value_type&& it): m_it(move(it)) { }
    
    constexpr auto& operator*() {
        return *prev(m_it);
    }

    constexpr auto& operator->() {
        return prev(m_it);
    }

    constexpr auto& operator++() {
        --m_it;
        return *this;
    }

    constexpr auto& operator--() {
        ++m_it;
        return *this;
    }

    constexpr friend bool operator!=(const self_type&, const self_type&);

private:
    value_type m_it;
};

template<typename Iter>
constexpr bool operator!=(const reverse_iterator<Iter>& lhs, 
                          const reverse_iterator<Iter>& rhs) 
{
    return lhs.m_it == rhs.m_it;
}

namespace detail {

template<typename Iter, typename Distance>
    requires random_access_iterator<Iter>
constexpr void advance(Iter& it, Distance n) {
    it += n;
}

template<typename Iter, typename Distance>
    requires bidirectional_iterator<Iter>
constexpr void advance(Iter& it, Distance n) {
    while(n < 0) {
        it--;
        n++;
    }

    while(n > 0) {
        it++;
        n--;
    }
}

template<typename Iter, typename Distance>
    requires forward_iterator<Iter>
constexpr void advance(Iter& it, Distance n) {
    for(Distance i = 0; i < n; i++)
        it++;
}

} // namespace detail

template<typename Iter, typename Distance> 
//    requires bidirectional_iterator<Iter> && std::is_integral<Distance>
constexpr void advance(Iter& it, Distance n) {
    detail::advance(it, n);
}

template<typename Iter> 
    requires bidirectional_iterator<Iter>
constexpr Iter prev(Iter it, 
                    typename iterator_traits<Iter>::difference_type n = 1)
{
    advance(it, -n);
    return it;
}

} // namespace nonstd
