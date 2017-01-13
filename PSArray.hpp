#ifndef PSARRAY_HPP_INCLUDED
#define PSARRAY_HPP_INCLUDED

#include "PSObject.hpp"
#include <cstdint>
#include <vector>
#include <initializer_list>

template<typename T>
class PSArray : public PSObject<std::vector<T, PSAllocator<T>>>
{
	private:
		typedef std::vector<T, PSAllocator<T>> underlying_type;
		typedef PSObject<underlying_type> parent_type;
		using parent_type::set_safe_size;
		using parent_type::set_size;
		using parent_type::Data;

	public:
		typedef typename underlying_type::size_type												size_type;
		typedef typename underlying_type::difference_type										difference_type;
		typedef typename underlying_type::pointer												pointer;
		typedef typename underlying_type::const_pointer											const_pointer;
		typedef typename underlying_type::reference												reference;
		typedef typename underlying_type::const_reference										const_reference;
		typedef typename underlying_type::value_type											value_type;
		typedef typename underlying_type::iterator												iterator;
		typedef typename underlying_type::const_iterator										const_iterator;
		typedef typename underlying_type::const_reverse_iterator								const_reverse_iterator;
		typedef typename underlying_type::reverse_iterator										reverse_iterator;

		explicit PSArray() : parent_type(underlying_type(PSAllocator<T>())) {set_safe_size();}
		explicit PSArray(size_type size) : parent_type(underlying_type(size, PSAllocator<T>())) {set_safe_size();}
		explicit PSArray(size_type size, const T& value) : parent_type(underlying_type(size, std::forward<decltype(value)>(value), PSAllocator<T>())) {set_safe_size();}

		template<class InputIt>
		PSArray(InputIt first, InputIt second) : parent_type(underlying_type(first, second, PSAllocator<T>())) {set_safe_size();}

		PSArray(const PSArray& other) : parent_type(underlying_type(other.Data)) {set_safe_size();}
		PSArray(PSArray&&  other) : parent_type(underlying_type(std::forward<decltype(other.Data)>(other.Data))) {set_safe_size();}
		PSArray(const std::initializer_list<T>& init) : parent_type(underlying_type(init, PSAllocator<T>())) {set_safe_size();}

		inline PSArray& operator = (const PSArray& other) { Data.operator = (std::forward<decltype(other.Data)>(other.Data)); set_safe_size(); return *this; }
		inline PSArray& operator = (std::initializer_list<T> ilist) { Data.operator = (std::forward<decltype(ilist)>(ilist)); set_safe_size(); return *this; }
		inline PSArray& operator = (PSArray&& other) { Data.operator = (std::forward<decltype(other.Data)>(other.Data)); set_safe_size(); return *this; }

		template<class InputIt>
		inline void assign(InputIt first, InputIt second) { Data.assign(first, second); set_size(); };
		inline void assign(size_type count, const T& value) { Data.assign(count, std::forward<decltype(value)>(value)); set_safe_size(); }
		inline void assign(std::initializer_list<T> ilist) { Data.assign(std::forward<decltype(ilist)>(ilist)); set_size(); set_safe_size(); }

		inline pointer data() { return Data.data(); }
		inline const_pointer data() const { return Data.data(); }

		inline iterator insert(const_iterator pos, const T& value) { iterator it = Data.insert(pos, std::forward<decltype(value)>(value)); set_size(); return it; }
		inline iterator insert(const_iterator pos, T&& value) { iterator it = Data.insert(pos, std::forward<decltype(value)>(value)); set_size(); return it; }
		inline iterator insert(const_iterator pos, size_type count, const T& value) { iterator it = Data.insert(pos, count, std::forward<decltype(value)>(value)); set_safe_size(); return it; }

		template<class InputIt>
		inline iterator insert(const_iterator pos, InputIt first, InputIt last) { iterator it = Data.insert(pos, first, last); set_size(); return it; }
		inline iterator insert(const_iterator pos, std::initializer_list<T> ilist) { iterator it = Data.insert(pos, std::forward<decltype(ilist)>(ilist)); set_safe_size(); return it; }

		template<class... Args>
		inline iterator emplace(iterator pos, Args&&  ... args) { iterator res = Data.emplace(pos, std::forward<Args>(args)...); set_safe_size(); return res; }
		template<class... Args>
		inline void emplace_back(Args&&  ... args) { Data.emplace_back(std::forward<Args>(args)...); set_safe_size(); }

		inline iterator erase(iterator pos) { iterator res = Data.erase(pos); set_safe_size(); return res; }
		inline iterator erase(iterator first, iterator last) { iterator res = Data.erase(first, last); set_safe_size(); return res; }

		inline void push_back(const T& value) { Data.push_back(std::forward<decltype(value)>(value)); set_size(); }
		inline void push_back(T&&  value) { Data.push_back(std::forward<decltype(value)>(value)); set_size(); }
		inline void pop_back() { Data.pop_back(); set_safe_size(); }

		inline void resize(size_type count, T value = T()) { Data.resize(count, std::forward<decltype(value)>(value)); set_safe_size(); }
		inline void swap(PSArray<T>& other) { Data.swap(other.Data); }
};

template<class T>
inline bool operator == (const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return (lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template<class T, class Alloc>
inline bool operator != (const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return !(lhs == rhs);
}

template<class T, class Alloc>
inline bool operator < (const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class T, class Alloc>
inline bool operator <= (const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return !(lhs < rhs);
}

template<class T, class Alloc>
inline bool operator >(const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return lhs < rhs;
}

template<class T, class Alloc>
inline bool operator >= (const PSArray<T>& lhs, const PSArray<T>& rhs)
{
	return !(lhs < rhs);
}

#endif // PSARRAY_HPP_INCLUDED
