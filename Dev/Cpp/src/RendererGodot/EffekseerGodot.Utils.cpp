
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerGodot.Utils.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
namespace Convert
{

size_t String16(char16_t* to, const godot::String& from, size_t size) {
#ifdef _MSC_VER
	// Simple copy
	const wchar_t* ustr = from.unicode_str();
	size_t len = (size_t)from.length();
	size_t count = std::min(len, size - 1);
	memcpy(to, ustr, count * sizeof(char16_t));
	to[count] = u'\0';
	return count;
#else
	// UTF32 -> UTF16
	const wchar_t* ustr = from.unicode_str();
	size_t len = (size_t)from.length();
	size_t count = 0;
	for (size_t i = 0; i < len; i++) {
		wchar_t c = ustr[i];
		if (c == 0) {
			break;
		}
		if ((uint32_t)c < 0x10000) {
			if (count >= size - 1) break;
			to[count++] = (char16_t)c;
		} else {
			if (count >= size - 2) break;
			to[count++] = (char16_t)(((uint32_t)c - 0x10000) / 0x400 + 0xD800);
			to[count++] = (char16_t)(((uint32_t)c - 0x10000) % 0x400 + 0xDC00);
		}
	}
	to[count] = u'\0';
	return count;
#endif
}

godot::String String16(const char16_t* from)
{
#ifdef _MSC_VER
	return godot::String((const wchar_t*)from);
#else
	godot::String result;
	while (true) {
		// FIXME
		wchar_t c[2] = {}; 
		c[0] = *from++;
		if (c[0] == 0) {
			break;
		}
		result += c;
	}
	return result;
#endif
}

}
} // namespace EffekseerGodot
