#ifndef PSSTRING_HPP_INCLUDED
#define PSSTRING_HPP_INCLUDED

#include "PSObject.hpp"
#include <string>
#include <stdexcept>

template<typename T, typename Traits = std::char_traits<T>>
class PSString : public PSObject<std::basic_string<T, Traits, PSAllocator<T>>>
{
    private:
        static constexpr std::int32_t size_offset = ((sizeof(std::int32_t) * 2) / sizeof(T));
        static constexpr std::int32_t index_offset = std::is_same<T, wchar_t>::value ? sizeof(T) : size_offset;
        typedef std::basic_string<T, std::char_traits<T>, PSAllocator<T>> underlying_type;
        typedef PSObject<underlying_type> parent_type;
        typedef Traits underlying_traits;
        using parent_type::Data;

    protected:
        virtual inline std::int32_t* size_ptr() { return reinterpret_cast<std::int32_t*>(&Data[0]); }
        virtual inline void set_size() { *size_ptr() = -1; *(size_ptr() + 1) = (Data.size() - size_offset) * sizeof(T); }
        virtual inline void set_safe_size() { if (Data.capacity()) { set_size(); } }

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
        static const size_type npos = -1;

        explicit PSString() : parent_type(underlying_type(size_offset, T())){set_safe_size();}
        PSString(size_type count, T ch) : parent_type(underlying_type(count + size_offset, ch)) {set_safe_size();}
        PSString(const PSString& other, size_type pos, size_type count = npos) : parent_type(underlying_type(size_offset, T())) {Data += other.substr(pos, count); set_safe_size();}
        PSString(const T* s, size_type count) : parent_type(underlying_type(size_offset, T())) {Data += underlying_type(s, count); set_safe_size();}
        PSString(const T* s) : parent_type(underlying_type(size_offset, T())) {Data += s; set_safe_size();}

        template<class InputIt>
        PSString(InputIt first, InputIt second) : parent_type(underlying_type(size_offset, T())) {Data += underlying_type(first, second); set_size();}
        PSString(const PSString& other) : parent_type(underlying_type(other.Data)) {set_safe_size();}
        PSString(PSString&& other) : parent_type(underlying_type(std::forward<decltype(other.Data)>(other.Data))) {set_safe_size();}
        PSString(const std::initializer_list<T>& init) : parent_type(underlying_type(size_offset, T())) {Data += init; set_safe_size();}

        inline PSString& operator = (const PSString& other)
        {
            Data.operator = (std::forward<decltype(other.Data)>(other.Data));
            set_safe_size();
            return *this;
        }

        inline PSString& operator = (PSString && other)
        {
            Data.operator = (std::forward<decltype(other.Data)>(other.Data));
            set_safe_size();
            return *this;
        }

        inline PSString& operator = (const T* s)
        {
            Data.operator = (s);
            set_safe_size();
            return *this;
        }

        inline PSString& operator = (T ch)
        {
            Data.operator = (ch);
            set_safe_size();
            return *this;
        }

        inline PSString& operator = (std::initializer_list<T> ilist)
        {
            Data.operator = (std::forward<decltype(ilist)>(ilist));
            set_safe_size();
            return *this;
        }

        inline virtual const void* address() const
        {
            return data();
        }

        virtual inline reference operator[](size_type pos)
        {
            return Data.operator[](pos + index_offset);
        }

        virtual inline const_reference operator[](size_type pos) const
        {
            return Data.operator[](pos + index_offset);
        }

        virtual inline pointer operator & ()
        {
            return &Data[size_offset];
        }

        inline void assign(size_type count, T ch)
        {
            Data.resize(count + size_offset, ch);
            set_safe_size();
        }

        inline void assign(const PSString& other)
        {
            Data.assign(other.Data);
            set_safe_size();
        }

        inline void assign(const PSString& other, size_type pos, size_type count)
        {
            Data.clear();
            Data.resize(size_offset);
            Data += other.substr(pos, count);
            set_safe_size();
        }

        inline void assign(PSString&& other)
        {
            Data.assign(std::forward<decltype(other.Data)>(other.Data));
            set_safe_size();
        }

        inline void assign(const T* s, size_type count)
        {
            Data.clear();
            Data.resize(size_offset);
            Data.append(s, count);
            set_safe_size();
        }

        inline void assign(const T* s)
        {
            Data.clear();
            Data.resize(size_offset);
            Data.append(s);
            set_safe_size();
        }

        template<class InputIt>
        inline void assign(InputIt first, InputIt second)
        {
            Data.clear();
            Data.resize(size_offset);
            Data.append(first, second);
            set_size();
        }

        inline void assign(std::initializer_list<T> ilist)
        {
            Data.clear();
            Data.resize(size_offset);
            Data.append(std::forward<decltype(ilist)>(ilist));
            set_safe_size();
        }

        inline const_pointer data() const
        {
            return Data.data() + size_offset;
            //return reinterpret_cast<const_pointer>(reinterpret_cast<const char*>(Data.data()) + size_offset);
        }

        inline const_pointer c_str() const
        {
            return Data.data() + size_offset;
            //return reinterpret_cast<const_pointer>(reinterpret_cast<const char*>(Data.c_str()) + size_offset);
        }

        inline size_type length() const
        {
            return Data.length() - size_offset;
        }

        inline virtual size_type size() const
        {
            return Data.size() - size_offset;
        }

        inline virtual bool empty() const
        {
            return size() == 0;
        }

        inline PSString& insert(size_type index, size_type count, T ch)
        {
            Data.insert(index + size_offset, count, ch);
            set_safe_size();
            return *this;
        }

        inline PSString& insert(size_type index, const T* s)
        {
            Data.insert(index + size_offset, s);
            set_safe_size();
            return *this;
        }

        inline PSString& insert(size_type index, const T* s, size_type count)
        {
            Data.insert(index + size_offset, s);
            set_safe_size();
            return *this;
        }

        inline PSString& insert(size_type index, const PSString& str)
        {
            Data.insert(index + size_offset, str.c_str(), str.Data.size() - size_offset);
            set_safe_size();
            return *this;
        }

        inline PSString& insert(size_type index, const PSString& str, size_type index_str, size_type count)
        {
            Data.insert(index + size_offset, str.c_str(), index_str, count);
            set_safe_size();
            return *this;
        }

        inline iterator insert(iterator pos, T ch)
        {
            iterator it = Data.insert(pos, ch);
            set_safe_size();
            return it;
        }

        inline void insert(iterator pos, size_type count, T ch)
        {
            Data.insert(pos, count, ch);
            set_safe_size();
        }

        template<class InputIt>
        inline void insert(iterator pos, InputIt first, InputIt last)
        {
            Data.insert(pos, first, last);
            set_size();
        }

        inline void insert(iterator pos, std::initializer_list<T> ilist)
        {
            Data.insert(pos, std::forward<decltype(ilist)>(ilist));
            set_safe_size();
        }

        inline PSString& erase(size_type index = 0, size_type count = npos)
        {
            Data.erase(index + size_offset, count);
            set_safe_size();
            return *this;
        }

        inline iterator erase(iterator position)
        {
            iterator it = Data.erase(position);
            set_safe_size();
            return it;
        }

        inline iterator erase(iterator first, iterator last)
        {
            iterator it = Data.erase(first + size_offset, last);
            set_safe_size();
            return it;
        }

        inline void push_back(T ch)
        {
            Data.push_back(ch);
            set_safe_size();
        }

        inline void pop_back()
        {
            if (Data.size() <= size_offset)
                throw std::out_of_range("basic_string::erase");
            Data.pop_back();
            set_safe_size();
        }

        inline PSString& append(size_type count, const T& ch)
        {
            Data.append(count, ch);
            set_safe_size();
            return *this;
        }

        inline PSString& append(const PSString& str)
        {
            Data.append(str.c_str(), str.size() - size_offset);
            set_safe_size();
            return *this;
        }

        inline PSString& append(const PSString& str, size_type pos, size_type count)
        {
            Data.append(str.c_str(), pos, count);
            set_safe_size();
            return *this;
        }

        inline PSString& append(const T* s, size_type count)
        {
            Data.append(s, count);
            set_safe_size();
            return *this;
        }

        inline PSString& append(const T* s)
        {
            Data.append(s);
            set_safe_size();
            return *this;
        }

        template<class InputIt>
        inline PSString& append(InputIt first, InputIt last)
        {
            Data.append(first, last);
            set_size();
            return *this;
        }

        inline PSString& append(std::initializer_list<T> ilist)
        {
            Data.append(std::forward<decltype(ilist)>(ilist));
            set_safe_size();
            return *this;
        }

        inline PSString& operator += (const PSString& str)
        {
            Data.operator += (str);
            set_safe_size();
            return *this;
        }

        inline PSString& operator += (T ch)
        {
            Data.operator += (ch);
            set_safe_size();
            return *this;
        }

        inline PSString& operator += (const T* s)
        {
            Data.operator += (s);
            set_safe_size();
            return *this;
        }

        inline PSString& operator += (std::initializer_list<T> ilist)
        {
            Data.operator += (std::forward<decltype(ilist)>(ilist));
            set_safe_size();
            return *this;
        }

        inline int compare(const PSString& str) const
        {
            return underlying_traits::compare(Data.c_str(), str.c_str(), std::min(Data.size(), str.size()));
        }

        inline int compare(size_type pos1, size_type count1, const PSString& str) const
        {
            auto&& tmp = PSString<T>(*this, pos1 + size_offset, count1);
            return underlying_traits::compare(tmp.c_str(), str.c_str(), std::min(tmp.size(), str.size()));
        }

        inline int compare(size_type pos1, size_type count1, const PSString& str, size_type pos2, size_type count2) const
        {
            auto&& tmp1 = PSString<T>(*this, pos1 + size_offset, count1);
            auto&& tmp2 = PSString<T>(str, pos2 + size_offset, count2);
            return underlying_traits::compare(tmp1.c_str(), tmp2.c_str(), std::min(tmp1.size(), tmp2.size()));
        }

        inline int compare(const T* s) const
        {
            return underlying_traits::compare(Data.c_str(), s, std::min(Data.size(), underlying_traits::length(s)));
        }

        inline int compare(size_type pos1, size_type count1, const T* s) const
        {
            auto&& tmp = PSString<T>(*this, pos1, count1);
            return underlying_traits::compare(tmp.c_str(), s, std::min(tmp.size(), underlying_traits::length(s)));
        }

        inline int compare(size_type pos1, size_type count1, const T* s, size_type count2) const
        {
            auto&& tmp1 = PSString<T>(*this, pos1, count1);
            auto&& tmp2 = PSString<T>(s, count2);
            return underlying_traits::compare(tmp1.c_str(), tmp2.c_str(), std::min(tmp1.size(), tmp2.size()));
        }

        inline PSString& replace(size_type pos, size_type count, const PSString& str)
        {
            Data.replace(pos + size_offset, count, str.c_str());
            set_safe_size();
            return *this;
        }

        inline PSString& replace(iterator first, iterator last, const PSString& str)
        {
            Data.replace(first, last, str.c_str());
            set_safe_size();
            return *this;
        }

        inline PSString& replace(size_type pos, size_type count, const PSString& str, size_type pos2, size_type count2)
        {
            Data.replace(pos + size_offset, count, str.c_str(), pos2, count2);
            set_safe_size();
            return *this;
        }

        template<class InputIt>
        inline PSString& replace(iterator first, iterator last, InputIt first2, InputIt last2)
        {
            Data.replace(first, last, first2, last2);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(iterator first, iterator last, const T* cstr, size_type count2)
        {
            Data.replace(first, last, cstr, count2);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(size_type pos, size_type count, const T* cstr)
        {
            Data.replace(pos + size_offset, count, cstr);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(iterator first, iterator last, const T* cstr)
        {
            Data.replace(first, last, cstr);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(size_type pos, size_type count, size_type count2, T ch)
        {
            Data.replace(pos + size_offset, count, count2, ch);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(iterator first, iterator last, size_type count2, T ch)
        {
            Data.replace(first, last, count2, ch);
            set_safe_size();
            return *this;
        }

        inline PSString& replace(iterator first, iterator last, std::initializer_list<T> ilist)
        {
            Data.replace(std::forward<decltype(ilist)>(ilist));
            set_safe_size();
            return *this;
        }

        inline PSString substr(size_type pos = 0, size_type count = npos) const
        {
            Data.substr(pos + size_offset, count);
            return *this;
        }

        inline size_type copy(T* dest, size_type count, size_type pos = 0) const
        {
            return Data.copy(dest, count, pos + size_offset);
        }

        inline void resize(size_type count)
        {
            Data.resize(count + size_offset);
            set_safe_size();
        }

        inline void resize(size_type count, T ch)
        {
            Data.resize(count + size_offset, ch);
            set_safe_size();
        }

        inline void swap(PSString& other)
        {
            Data.swap(other);
        }

        inline size_type find(const PSString& str, size_type pos = 0) const
        {
            size_type result = Data.find(str.c_str(), pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find(const T* s, size_type pos, size_type count) const
        {
            size_type result = Data.find(s, pos + size_offset, count);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find(const T* s, size_type pos = 0) const
        {
            size_type result = Data.find(s, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find(T ch, size_type pos = 0) const
        {
            size_type result = Data.find(ch, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type rfind(const PSString& str, size_type pos = npos) const
        {
            return underlying_type(Data.c_str(), Data.size()).rfind(str.c_str(), pos);
        }

        inline size_type rfind(const T* s, size_type pos, size_type count) const
        {
            return underlying_type(Data.c_str(), Data.size()).rfind(s, pos, count);
        }

        inline size_type rfind(const T* s, size_type pos = npos) const
        {
            return underlying_type(Data.c_str(), Data.size()).rfind(s, pos);
        }

        inline size_type rfind(T ch, size_type pos = 0) const
        {
            return underlying_type(Data.c_str(), Data.size()).rfind(ch, pos);
        }

        inline size_type find_first_of(const PSString& str, size_type pos = 0) const
        {
            size_type result = Data.find_first_of(str.c_str(), pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_of(const T* s, size_type pos, size_type count) const
        {
            size_type result = Data.find_first_of(s, pos + size_offset, count);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_of(const T* s, size_type pos = 0) const
        {
            size_type result = Data.find_first_of(s, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_of(T ch, size_type pos = 0) const
        {
            size_type result = Data.find_first_of(ch, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_not_of(const PSString& str, size_type pos = 0) const
        {
            size_type result = Data.find_first_not_of(str.c_str(), pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_not_of(const T* s, size_type pos, size_type count) const
        {
            size_type result = Data.find_first_not_of(s, pos + size_offset, count);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_not_of(const T* s, size_type pos = 0) const
        {
            size_type result = Data.find_first_not_of(s, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_first_not_of(T ch, size_type pos = 0) const
        {
            size_type result = Data.find_first_not_of(ch, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_of(const PSString& str, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_of(str.c_str(), pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_of(const T* s, size_type pos, size_type count) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_of(s, pos + size_offset, count);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_of(const T* s, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_of(s, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_of(T ch, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_of(ch, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_not_of(const PSString& str, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_not_of(str.c_str(), pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_not_of(const T* s, size_type pos, size_type count) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_not_of(s, pos + size_offset, count);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_not_of(const T* s, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_not_of(s, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        inline size_type find_last_not_of(T ch, size_type pos = 0) const
        {
            size_type result = underlying_type(Data.c_str(), Data.size()).find_last_not_of(ch, pos + size_offset);
            return result == npos ? npos : result - size_offset;
        }

        template<typename U>
        inline friend std::basic_ostream<U, std::char_traits<U>>& operator << (std::basic_ostream<U, std::char_traits<U>>& os, const PSString<U>& str);

        template<typename U>
        inline friend std::basic_istream<U, std::char_traits<U>>& operator >> (std::basic_istream<U, std::char_traits<U>>& is, PSString<U>& str);
};

template<class T>
inline PSString<T> operator + (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).append(rhs);
}

template<class T>
inline PSString<T> operator + (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs) + rhs;
}

template<class T>
inline PSString<T> operator + (T lhs, const PSString<T>& rhs)
{
    return PSString<T>(1, lhs) + rhs;
}

template<class T>
inline PSString<T> operator + (const PSString<T>& lhs, const T* rhs)
{
    return lhs + PSString<T>(rhs);
}

template<class T>
inline PSString<T> operator + (const PSString<T>& lhs, T rhs)
{
    return lhs + PSString<T>(1, rhs);
}

template<class T>
inline PSString<T> operator + (PSString<T>&& lhs, const PSString<T>& rhs)
{
    return std::move(lhs.append(rhs));
}

template<class T>
inline PSString<T> operator + (const PSString<T>& lhs, PSString<T>&& rhs)
{
    return std::move(rhs.insert(0, lhs));
}

template<class T>
inline PSString<T> operator + (PSString<T>&& lhs, PSString<T>&& rhs)
{
    return std::move(lhs.append(rhs));
}

template<class T>
inline PSString<T> operator + (const T* lhs, PSString<T>&& rhs)
{
    return std::move(rhs.insert(0, lhs));
}

template<class T>
inline PSString<T> operator + (T lhs, PSString<T>&& rhs)
{
    return std::move(rhs.insert(0, 1, lhs));
}

template<class T>
inline PSString<T> operator + (PSString<T>&& lhs, const T* rhs)
{
    return std::move(lhs.append(rhs));
}

template<class T>
inline PSString<T> operator + (PSString<T>&& lhs, T rhs)
{
    return std::move(lhs.append(1, rhs));
}

template<class T>
inline bool operator == (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator != (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator < (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator <= (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator > (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator >= (const PSString<T>& lhs, const PSString<T>& rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator == (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator == (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator != (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator != (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator < (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator < (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator <= (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator <= (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator > (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator > (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline bool operator >= (const T* lhs, const PSString<T>& rhs)
{
    return PSString<T>(lhs).compare(rhs);
}

template<class T>
inline bool operator >= (const PSString<T>& lhs, const T* rhs)
{
    return lhs.compare(rhs);
}

template<class T>
inline void swap(PSString<T>& lhs, PSString<T>& rhs)
{
    lhs.swap(rhs);
}

template<class T>
inline std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& os, const PSString<T>& str)
{
    os.write(str.c_str(), str.size());
    return os;
}

template <class T>
inline std::basic_istream<T, std::char_traits<T>>& operator >> (std::basic_istream<T, std::char_traits<T>>& is, PSString<T>& str)
{
    std::basic_istream<T, std::char_traits<T>>& ret = (is >> str.Data);
    str.set_safe_size();
    return ret;
}

#endif // PSSTRING_HPP_INCLUDED
