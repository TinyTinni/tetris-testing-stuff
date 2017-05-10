#pragma once
#ifndef __ARRAY_ACCESSOR_HPP__
#define __ARRAY_ACCESSOR_HPP__


template<typename T, size_t At, typename Cmp>
class ArrayAccessor
{
	public:
		ArrayAccessor(){}
		auto operator()(const T& lhs, const T& rhs) -> decltype(Cmp()(lhs[At],rhs[At]))
		{
			return Cmp()(lhs[At],rhs[At]);
		}
};


#endif //__ARRAY_ACCESSOR_HPP__