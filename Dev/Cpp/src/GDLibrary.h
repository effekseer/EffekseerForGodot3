#pragma once

#include <Godot.hpp>
#include <Effekseer.h>

template <size_t SIZE>
struct FixedString
{
	char str[SIZE];
	FixedString(godot::String from) {
#ifdef _MSC_VER
		size_t outsize;
		wcstombs_s(&outsize, str, from.unicode_str(), SIZE);
#else
		wcstombs(str, from.unicode_str(), SIZE);
#endif
	}
};
