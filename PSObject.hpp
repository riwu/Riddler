#ifndef PSOBJECT_HPP_INCLUDED
#define PSOBJECT_HPP_INCLUDED

#include <cstdint>
#include <iterator>

template <typename T>
struct PSAllocator
{
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template<typename U>
    struct rebind {typedef PSAllocator<U> other;};

    PSAllocator() throw() {};
    PSAllocator(const PSAllocator& other) throw() {};

    template<typename U>
    PSAllocator(const PSAllocator<U>& other) throw() {};

    template<typename U>
    PSAllocator& operator = (const PSAllocator<U>& other) { return *this; }
    PSAllocator<T>& operator = (const PSAllocator& other) { return *this; }
    ~PSAllocator() {}

    pointer allocate(size_type n, const void* hint = 0)
    {
        std::int32_t* data_ptr = reinterpret_cast<std::int32_t*>(::operator new((n * sizeof(value_type)) + (sizeof(std::int32_t) * 2)));
        return reinterpret_cast<pointer>(&data_ptr[2]);
    }

    void deallocate(T* ptr, size_type n)
    {
        std::int32_t* data_ptr = reinterpret_cast<std::int32_t*>(ptr);
        ::operator delete(reinterpret_cast<T*>(&data_ptr[-2]));
    }
};

template <typename T, typename U>
inline bool operator == (const PSAllocator<T>&, const PSAllocator<U>&)
{
    return true;
}

template <typename T, typename U>
inline bool operator != (const PSAllocator<T>& a, const PSAllocator<U>& b)
{
    return !(a == b);
}

class PSBaseObject
{
	public:
		PSBaseObject() {}
		virtual ~PSBaseObject() {}

		inline virtual const void* address() const { return this; }
};

template<typename T>
class PSNativeObject : public PSBaseObject
{
	private:
		T* data_;

	public:
		PSNativeObject(T* object) : data_(object) {}
		~PSNativeObject() { delete data_; }

		inline virtual const void* address() const { return data_; }
};

template<typename T>
class PSObject : public PSBaseObject
{
	protected:
		T Data;
		virtual inline std::int32_t* size_ptr() { return (reinterpret_cast<std::int32_t*>(&Data[0]) - 1); }
		virtual inline void set_size() { *(size_ptr() - 1) = -1; (*size_ptr() = Data.size() - 1); }
		virtual inline void set_safe_size() {if (Data.capacity()) {set_size();}}

	public:
		typedef typename T::size_type											size_type;
		typedef typename T::difference_type										difference_type;
		typedef typename T::pointer												pointer;
		typedef typename T::const_pointer										const_pointer;
		typedef typename T::reference											reference;
		typedef typename T::const_reference										const_reference;
		typedef typename T::value_type											value_type;
		typedef typename T::iterator											iterator;
		typedef typename T::const_iterator										const_iterator;
		typedef typename T::const_reverse_iterator								const_reverse_iterator;
		typedef typename T::reverse_iterator									reverse_iterator;

		PSObject(const T& Data) : Data(Data) {}
		PSObject(T&& Data_) : Data(std::forward<decltype(Data_)>(Data_)) {}
		virtual ~PSObject() {}

		inline PSAllocator<T> get_allocator() const { return Data.get_allocator(); }
		inline reference at(size_type pos) { return Data.at(pos); }
		inline const_reference at(size_type pos) const { return Data.at(pos); }
        inline virtual reference operator[](size_type pos) { return Data.operator[](pos); }
		inline virtual const_reference operator[](size_type pos) const { return Data.operator[](pos); }
		inline reference front() { return Data.front(); }
		inline const const_reference front() const { return Data.front(); }
		inline reference back() { return Data.back(); }
		inline const const_reference back() const { return Data.back(); }

		inline iterator begin() { return Data.begin(); }
		inline const_iterator begin() const { return Data.begin(); }
		inline const_iterator cbegin() const { return Data.cbegin(); }
		inline iterator end() { return Data.end(); }
		inline const_iterator end() const { return Data.end(); }
		inline const_iterator cend() const { return Data.cend(); }
		inline reverse_iterator rbegin() { return Data.rbegin(); }
		inline const_reverse_iterator rbegin() const { return Data.rbegin(); }
		inline const_reverse_iterator crbegin() const { return Data.rbegin(); }
		inline reverse_iterator rend() { return Data.rend(); }
		inline const_reverse_iterator rend() const { return Data.rend(); }
		inline const_reverse_iterator crend() const { return Data.crend(); }

		inline virtual bool empty() const { return Data.empty(); }
		inline virtual size_type size() const { return Data.size(); }
		inline size_type max_size() const { return Data.max_size(); }
		inline void reserve(size_type new_cap) { Data.reserve(new_cap); }
		inline size_type capacity() const { return Data.capacity(); }
		inline void shrink_to_fit() { Data.shrink_to_fit(); }
		inline void clear() { Data.clear(); set_safe_size(); }

		inline virtual const void* address() const { return Data.data(); }
		inline virtual pointer operator & () { return &Data[0]; }
};

#endif // PSOBJECT_HPP_INCLUDED
