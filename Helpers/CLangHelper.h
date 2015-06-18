#pragma once

#include "EnginePCH.h"

template <typename A>
typename std::enable_if <std::is_array <A> ::value, size_t> ::type
	SizeOfArray(const A& a)
{
	return std::extent <A> ::value;
}

template<typename T>
void FastErase(std::vector<T>& v, size_t inx)
{
	std::swap(v[inx], v.back());
	v.pop_back();
}