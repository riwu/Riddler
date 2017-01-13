#ifndef GC_HPP_INCLUDED
#define GC_HPP_INCLUDED

#include <vector>
#include <memory>
#include <algorithm>
#include <cstdint>
#include "PSObject.hpp"
#include "PSString.hpp"
#include "PSArray.hpp"
#include <functional>

class GC
{
	private:
		std::vector<std::unique_ptr<PSBaseObject, std::function<void(PSBaseObject*)>>> Objects;

	public:
		~GC() { Clear(); }

		template<typename T>
		T* Add(T* object)
		{
			auto deleter = [](PSBaseObject* ptr){delete ptr; };
			this->Objects.emplace_back(std::unique_ptr<PSBaseObject, decltype(deleter)>(object, deleter));
			return object;
		}

		template<typename T>
		T* Add(T* object, const std::function<void(T*)> &deleter)
		{
			this->Objects.emplace_back(std::unique_ptr<PSBaseObject, decltype(deleter)>(object, deleter));
			return object;
		}

		template<typename T>
		bool Remove(T* object)
		{
			auto it = std::find_if(this->Objects.begin(), this->Objects.end(), [&](const decltype(Objects)::value_type& it) { return it.get()->address() == object; });
			if (it != this->Objects.end())
			{
				this->Objects.erase(it);
				return true;
			}
			return false;
		}

		template<typename T>
		std::unique_ptr<PSBaseObject> RemoveAlive(T* object)
		{
			auto it = std::find_if(this->Objects.begin(), this->Objects.end(), [&](const decltype(Objects)::value_type& it) { return it.get()->address() == object; });
			if (it != this->Objects.end())
			{
				auto res = std::move(*it);
				this->Objects.erase(it);
				return res;
			}
		}

		inline void Clear() { Objects.clear(); Objects.shrink_to_fit(); decltype(Objects)().swap(Objects); }

		inline std::size_t count() {return Objects.size();}
};

#endif // GC_HPP_INCLUDED
