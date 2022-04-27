#pragma once

#include <cstdint>
#include <climits>

#include <initializer_list>

#include "iterator.hpp"
#include "utility.hpp"
#include "allocator.hpp"
#include "memory.hpp"

#ifndef NO_DISCARD
#   define NO_DISCARD [[nodiscard]]
#endif

namespace kstd {

template<typename T> struct doubly_linked_list_node {
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = doubly_linked_list_node<value_type>*;
	using const_pointer = const doubly_linked_list_node<value_type>*;
	using self_type = doubly_linked_list_node<value_type>;

	pointer prev;	// pointer to previous node, if any
	value_type val;	// value held by node
	pointer next;	// pointer to next node, if any

	doubly_linked_list_node() : prev(nullptr), val(), next(nullptr) { }
	doubly_linked_list_node(pointer _prev, pointer _next = nullptr) 
        : prev(_prev), val(), next(_next) { }
	doubly_linked_list_node(const_pointer _prev, const_pointer _next = nullptr)
        : prev(const_cast<pointer>(_prev)), 
          val(), 
          next(const_cast<pointer>(_next)) 
    { 

    }
	doubly_linked_list_node(const_reference _val, 
                            const_pointer _prev = nullptr, 
                            const_pointer _next = nullptr) 
        : prev(static_cast<pointer>(_prev)), 
          val(_val), 
          next(static_cast<pointer>(_next)) 
    { 

    }
	doubly_linked_list_node(value_type&& _val, 
                            const_pointer _prev = nullptr, 
                            const_pointer _next = nullptr) 
        : prev(static_cast<pointer>(_prev)), 
          val(forward<value_type>(_val)), 
          next(static_cast<pointer>(_next)) 
    {

    }
	
    doubly_linked_list_node(const self_type& other) { 
        prev = other.prev; 
        val = other.val; 
        next = other.next; 
    }
	doubly_linked_list_node(self_type&& other) 
        : prev(move(other.prev)), 
          val(move(other.val)), 
          next(move(other.next)) 
    { 
        other.prev = nullptr; 
        other.val = value_type(); 
        other.next = nullptr; 
    }


	// copy assignment
	self_type& operator=(const self_type& rhs) {
		prev = rhs.prev;
		val = rhs.val;
		next = rhs.next;
		return *this;
	}

	// move assignment
	self_type& operator=(self_type&& rhs) noexcept {
		if (this != &rhs) {
			prev = move(rhs.prev);
			val = move(rhs.val);
			next = move(rhs.next);

			rhs.prev = nullptr;
			rhs.val = value_type();
			rhs.next = nullptr;
		}
		return *this;
	}
};

template<typename T> 
struct doubly_linked_list_iterator 
    : public iterator<bidirectional_iterator_tag, T> 
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = intmax_t;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using node_pointer = doubly_linked_list_node<value_type>*;
	using const_node_pointer = const doubly_linked_list_node<value_type>*;
	using node_pointer_const = doubly_linked_list_node<const value_type>*;
	using node_reference = doubly_linked_list_node<value_type>&;
	using node_const_reference = const doubly_linked_list_node<const value_type>&;
	using const_node_reference = const doubly_linked_list_node<value_type>&;
	using const_node_const_reference = const doubly_linked_list_node<const value_type>&;
	using iterator_category = bidirectional_iterator_tag;
	using self_type = doubly_linked_list_iterator<value_type>;

	constexpr doubly_linked_list_iterator() : m_ptr(nullptr) { }
	constexpr doubly_linked_list_iterator(const self_type&) = default; 
	constexpr doubly_linked_list_iterator(self_type&&) = default; 

	constexpr doubly_linked_list_iterator(const_node_pointer t) 
        : m_ptr((node_pointer)t) { }
	constexpr doubly_linked_list_iterator(const_node_reference t) 
        : m_ptr(&static_cast<node_reference>(t)) { }
	constexpr doubly_linked_list_iterator(const_node_const_reference t) requires (!same_as<value_type, const value_type>)
        : m_ptr((node_pointer)&t) { }

	constexpr self_type& operator--() {
		m_ptr = m_ptr->prev;
		return *this;
	}

	constexpr const self_type& operator--() const {
		m_ptr = m_ptr->prev;
		return *this;
	}

    constexpr self_type operator--(int) {
        self_type& temp = *this;
		m_ptr = m_ptr->prev;
		return temp;
	}

	constexpr const self_type& operator--(int) const {
        self_type& temp = *this;
		m_ptr = m_ptr->prev;
		return temp;
	}
	
	constexpr self_type& operator++() {
		m_ptr = m_ptr->next;
		return *this;
	}

	constexpr const self_type& operator++() const {
		return *this;
	}

    constexpr self_type& operator++(int) {
        self_type& temp = *this;
		m_ptr = m_ptr->next;
		return temp;
	}

	constexpr const self_type& operator++(int) const {
        self_type& temp = *this;
		m_ptr = m_ptr->next;
		return temp;
	}

	constexpr self_type& operator=(const node_pointer rhs) {
		m_ptr = rhs;
	}

	constexpr const self_type& operator=(const_node_pointer rhs) const& {
		m_ptr = static_cast<node_pointer>(rhs);
	}
	
	constexpr self_type& operator=(const_node_reference rhs) & {
		m_ptr = &rhs;
	}

	constexpr const self_type& operator=(const_node_reference rhs) const& {
		m_ptr = static_cast<node_pointer>(&rhs);
	}

	constexpr self_type& operator=(const self_type& rhs) & {
		m_ptr = static_cast<node_pointer>(rhs.const_pointer());
	}

	constexpr const self_type& operator=(const self_type& rhs) const& {
		m_ptr = static_cast<node_pointer>(rhs.const_pointer());
	}

	constexpr reference operator*() & {
		return m_ptr->val;
	}

	constexpr const_reference operator*() const& {
		return m_ptr->val;
	}
	
	constexpr node_pointer operator->() {
		return m_ptr;
	}

	constexpr const_node_pointer operator->() const {
		return m_ptr;
	}

	constexpr node_pointer ptr() {
		return m_ptr;
	}

	constexpr const_node_pointer const_ptr() const {
		return m_ptr;
	}

	constexpr operator value_type() const {
		return m_ptr->val;
	}

protected:
	mutable node_pointer m_ptr = nullptr;
};

template<typename T> 
constexpr inline bool operator==(
    const doubly_linked_list_iterator<T>& lhs,
    const doubly_linked_list_iterator<T>& rhs) 
{
	return lhs.const_ptr() == rhs.const_ptr();
}

template<typename T> 
constexpr inline bool operator!=(
        const doubly_linked_list_iterator<T>& lhs, 
        const doubly_linked_list_iterator<T>& rhs) 
{
	return lhs.const_ptr() != rhs.const_ptr();
}


template<typename T, class Allocator = allocator<doubly_linked_list_node<T>>>
class list {
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = doubly_linked_list_iterator<value_type>;
	using const_iterator = doubly_linked_list_iterator<const value_type>;
	using const_reverse_iterator =  reverse_iterator<const_iterator>;
	using reverse_iterator = reverse_iterator<iterator>;
	using allocator_type = Allocator;

    using node_type = doubly_linked_list_node<value_type>;
	using node_pointer = node_type*;
	using const_node_pointer = const node_type*;
	using traits = allocator_traits<allocator_type>;
    using self_type = list<value_type, allocator_type>;

	// default constructor
	list() 
		: m_base(nullptr),
		  m_head(nullptr),
		  m_past_begin(),
		  m_past_end(),
		  m_alloc(allocator_type()),
		  m_size(0)
	{
	
	}

	// default constructor with supplied allocator
	explicit list(const allocator_type& alloc) 
		: m_base(nullptr),
		  m_head(nullptr),
		  m_past_begin(),
		  m_past_end(),
		  m_alloc(alloc),
		  m_size(0)
	{ 
	
	}

	// initialize a specific number of elements to a given value with a given allocator
	list(size_type count, 
         const_reference value = T(), 
         const allocator_type& alloc = allocator_type()) 
		: m_past_begin(),
		  m_past_end(),
		  m_base(nullptr),
		  m_head(nullptr), 
		  m_alloc(alloc),
		  m_size(0)
	{
		assign(count, value);
	}

	// default-initialize a specific number of elements
	explicit list(size_type count, allocator_type& alloc = allocator_type()) 
		: m_past_begin(),
		  m_past_end(),
		  m_base(nullptr),
		  m_head(nullptr),
		  m_alloc(alloc),
		  m_size(0)
	{
		assign(count, value_type());
	}

    template<typename Iter> 
        requires input_iterator<Iter>
    list(Iter first, Iter last, allocator_type& alloc = allocator_type())
        : m_alloc(alloc)
    {
        assign(first, last);
    }

	// copy constructor
	list(const list& other)
		: m_past_begin(),
		  m_past_end(),
		  m_base(nullptr),
		  m_head(nullptr),
		  m_alloc(allocator_type()),
		  m_size(0)
	{
	    assign(other);
    }

	// copy constructor with supplied allocator
	list(const list& other, const allocator_type& alloc)
		: m_past_begin(),
		  m_past_end(),
		  m_base(nullptr),
		  m_head(nullptr),
		  m_alloc(alloc),
		  m_size(0)
	{
        assign(other);    
	}

	// move constructor
	list(list&& other) 
		: m_past_begin(move(other.m_past_begin)),
		  m_past_end(move(other.m_past_end)),
		  m_base(move(other.m_base)),
		  m_head(move(other.m_head)),
		  m_alloc(move(other.m_alloc)),
		  m_size(move(other.m_size))
	{
	    assign(forward<self_type>(other));
    }

	// move constructor with supplied allocator
	list(list&& other, const allocator_type& alloc) 
		: m_past_begin(other.m_past_begin),
		  m_past_end(other.m_past_end),
		  m_base(other.m_base),
		  m_head(other.m_head),
		  m_alloc(alloc),
		  m_size(other.m_size)
	{
	    assign(forward<self_type>(other));
    }

	// initializer list constructor with supplied allocator
	list(std::initializer_list<value_type> ilist, 
        const allocator_type& alloc = allocator_type()) 
		: m_past_begin(),
		  m_past_end(),
		  m_base(nullptr),
		  m_head(nullptr),
		  m_alloc(alloc),
		  m_size(0)
	{
		assign(ilist);
	}

	// construct list with elements in range [first, last) using supplied allocator
	template<class Iter> 
        requires input_iterator<Iter>
    list(Iter first, Iter last, const allocator_type& alloc = allocator_type())
	    : m_past_begin(),
		  m_past_end(),
		  m_alloc(alloc) 
    {
	    assign(first, last);
    }

	~list() {
		clear();
	}

	// copy assignment operator
	self_type& operator=(const list& rhs) {
		assign(rhs);
        return *this;
	}

	// move assignment operator
	self_type& operator=(list&& rhs) {
		assign(forward<self_type>(rhs));
        return *this;
	}

	// initializer list assignment operator
	self_type& operator=(std::initializer_list<value_type> ilist) {
		assign(ilist);
	}

	// clear container and replace with count copies of value
	void assign(size_type count, const_reference value) {
		clear();
		for (std::size_t i = 0; i < count; i++)
			push_back(value);
	}

	// clear contents and insert elements in range [first, last)
	template<class Iter> 
        requires input_iterator<Iter>
    void assign(Iter first, Iter last) {
		clear();
		for (; first != last; ++first)
			push_back(*first);
	}

	// clear contents and insert elements in ilist
	void assign(std::initializer_list<value_type> ilist) {
		clear();
		for (const auto& r : ilist)
			push_back(r);
	}

	// return allocator
	allocator_type& get_allocator() const noexcept {
		return m_alloc;
	}

	// return value at front of container
	reference front() {
		return m_base->val;
	}

	const_reference front() const {
		return m_base->val;
	}

	// return value at back of container
	reference back() {
		return m_head->val;
	}

	const_reference back() const {
		return m_head->val;
	}

	// return iterator to front of container
	iterator begin() noexcept {
		return iterator((const_pointer)m_base);
	}

	// return const iterator to front of container
	const_iterator begin() const noexcept {
		return const_iterator((typename const_iterator::const_node_pointer)m_base);
	}

	const_iterator cbegin() const noexcept {
		return const_iterator(begin());
	}

	// return reverse iterator to front of container
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	// return const reverse iterator to front of container
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(rbegin());
	}

	// return iterator to one element past the end
	iterator end() noexcept {
		return iterator(&m_past_end);
	}

	// return const iterator to one element past the end
	const_iterator end() const noexcept {
		return const_iterator((typename const_iterator::const_node_pointer)&m_past_end);
	}

	const_iterator cend() const noexcept {
		return const_iterator(end());
	}

	// return reverse iterator to one element past the end
	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	// return iterator to one element past the end
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(rend());
	}

	// check whether container is empty (contains zero elements)
	NO_DISCARD bool empty() const noexcept {
		return size() == 0;
	}

	// return number of elements
	size_type size() const noexcept {
		return m_size;
	}

	// return max number of elements
	size_type max_size() const noexcept {
		return UINT_MAX;
	}

	// erase all elements in container
	void clear() noexcept {
        // nothing to clear if empty
		if (empty())
			return;

        // erase all elements
		erase(cbegin(), cend());
        // and null the relevant pointers
        m_head = nullptr;
		m_base = nullptr;
		m_past_begin.next = nullptr;
		m_past_end.prev = nullptr;
	}

	// insert new element before pos
    iterator insert(const_iterator pos, value_type&& value) {
        insert(pos, forward<value_type>(value));
    }

	iterator insert(const_iterator pos, const_reference value) {
		node_pointer temp = traits::allocate(sizeof(node_type));
		if (empty()) {
			traits::construct(m_alloc, 
                              temp, 
                              value, 
                              &m_past_begin, 
                              &m_past_end);
			m_base = temp;
			m_head = temp;
			m_past_begin.next = temp;
			m_past_end.prev = temp;
			m_size = 1;
			return iterator(temp);
		}

		traits::construct(m_alloc, temp, value, pos->prev, pos.const_ptr());
		temp->prev->next = temp;
		temp->next->prev = temp;
		m_size++;

		if (pos == cbegin())
			m_base = pos->prev;

		return iterator(temp);
	}

	// insert elements in range [first, last) before pos
	template<class Iter>
        requires input_iterator<Iter>
    iterator insert(const_iterator pos, Iter first, Iter last) {
		for (; first != last; ++first)
			insert(pos, *first);
		return pos;
	}

	// insert elements in ilist before pos
	iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
		for (const auto& r : ilist)
			insert(pos, r);
		return pos;
	}

	// construct node in-place before pos
	template<typename ...Args> iterator emplace(const_iterator pos,
                                                Args&&... args) 
    {
		node_pointer temp = traits::allocate(sizeof(node_type));
		node_type value(forward<Args>(args)...);
		if (empty()) {
			traits::construct(m_alloc, temp, value, &m_past_begin, &m_past_end);
			m_base = temp;
			m_head = temp;
			m_past_begin.next = temp;
			m_past_end.prev = temp;
			m_size = 1;
			return iterator(temp);
		}

		traits::construct(m_alloc, temp, value, pos->prev, pos.const_ptr());
		temp->prev->next = temp;
		temp->next->prev = temp;
		m_size++;
		return iterator(temp);
	}

	// erase element at pos and return iterator to next element
	iterator erase(const_iterator pos) {
		if (pos == cend())
			return iterator(nullptr); // can't erase end() 
        // don't dereference nullptr, 
		if (pos.const_ptr() == nullptr)			
            return iterator(nullptr); 

		pos->prev->next = pos->next;
		pos->next->prev = pos->prev;
		iterator it(pos->next);
		traits::destroy(m_alloc, pos.const_ptr());
		traits::deallocate((node_pointer)pos.const_ptr(), sizeof(*pos.ptr()));

		m_size--;

		return it;
	}

	// erase elements in range [first, last) and return iterator to last
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last) {
			++first;
			erase(first->prev);
		}
		return iterator(last.const_ptr());
	}

	// add new element after last element
	void push_back(const_reference value) {
		insert(cend(), value);
	}

	// move new element to after last element
	void push_back(value_type&& value) {
		insert(cend(), move(value));
		value = value_type();
	}

	// emplace new element after last element
	template<typename ...Args> reference emplace_back(Args&&... args) {
		emplace(cend(), forward<Args>(args)...);
		return *(--cend());
	}

	// remove last element
	void pop_back() {
		erase(--end());
	}

	// add element before first element
	void push_front(const_reference value) {
		insert(cbegin(), value);
	}

	// move new element before first element
	void push_front(value_type&& value) {
		insert(cbegin(), move(value));
		value = value_type();
	}

	// emplace new element before first element
	template<typename ...Types> reference emplace_front(Types&&... args) {
		emplace(cbegin(), forward<Types>(args)...);
		return *cbegin();
	}
	
	// remove first element
	void pop_front() {
		auto old_base = *m_base;
		erase(begin());
		m_base = old_base.next;
	}

	// add size number of default-initialized elements to end of container
	void resize(std::size_t size) {
		resize(size, value_type());
	}

	// add size number of elements initialized to value to end of container
    void resize(size_type size, const_reference value) {
        // if new size is equal to current size, nothing to be done
		if (m_size == size)				
            return;

        // if current size is less than new size
        // append (size - m_size) elements initialized to value
		if (m_size < size) { 			
            for (std::size_t i = 0; i < (size - m_size); i++)
				push_back(value);
			return;
		}

		// else pop_back() until size == m_size
		for (int i = 0; i < (m_size - size); i++)
			pop_back();
		return;
	}

	// swap contents of list with another list
	void swap(list& other) {
		list<value_type> temp = *this;
		*this = other;
		other = temp;
	}

	// TODO - merge
	// TODO - splice
	// TODO - remove
	// TODO - remove_if
	// TODO - reverse
	// TODO - unique
	// TODO - sort

protected:
	node_pointer m_base;    // pointer to last element (end)
	node_pointer m_head;    // pointer to first element (beginning)
	node_type m_past_begin;	// element past beginning 
                            // (used to prevent dereferencing nullptr)
	node_type m_past_end;	// element past end 
                            // (used to prevent dereferencing nullptr)
	allocator_type m_alloc;	// allocator type
	size_type m_size;	    // number of elements
};

template<typename Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
    requires input_iterator<Iter>
list(Iter, Iter, Alloc = Alloc())
  -> list<typename iterator_traits<Iter>::value_type, Alloc>;

} // namespace kstd
