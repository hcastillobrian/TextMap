#pragma once
#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <vector>
#include <string>
#include <map>

namespace FFe
{
	template<class T> inline void SafeDelete(T*& pVal) { if (pVal) { delete pVal; pVal = nullptr; } }
	template<class T> inline void SafeDeleteArray(T*& pVal) { if (pVal) { delete[] pVal; pVal = nullptr; } }
	template<class T> inline void SafeRelease(T*& pVal) { if (pVal) { pVal->Release(); pVal = nullptr; } }

	typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> > String;

	template <typename T>
	class Vector : public std::vector<T, std::allocator<T> >
	{
	public:
		Vector() : std::vector<T, std::allocator<T> >() {}
		Vector(size_type count) : std::vector<T, std::allocator<T> >(count) {}
		Vector(size_type count, const T& value) : std::vector<T, std::allocator<T> >(count, value) {}
		Vector(std::initializer_list<T> args) : std::vector<T, std::allocator<T> >(args) {}
	};

	template <typename TK, typename T>
	class Map : public std::map<TK, T, std::less<TK>, std::allocator< std::pair<const TK, T> > > {};

	template<typename T>
	struct Singleton
	{
		static inline T& Instance()
		{
			static T instance;
			return instance;
		}
	};

	template<typename T>
	class Instance
	{
		unsigned nInstance;
	public:
		T* InstanceAdd()
		{
			nInstance++;
			return (T*)this;
		}
		void InstanceSet(unsigned n)
		{
			nInstance = n;
		}
		unsigned InstanceGet()
		{
			return nInstance;
		}
		unsigned InstanceSub()
		{
			if (nInstance)
			{
				nInstance--;
			}
			return nInstance;
		}
		Instance()
		{
			nInstance = 1;
		}
		void Release()
		{
			if (InstanceSub() <= 0)
			{
				delete (T*)(this);
			}
		}
	};

	inline void StringReplace(String& str, const String& from, const String& to)
	{
		if (from.size())
		{
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != std::string::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length();
			}
		}
	}

}

#endif