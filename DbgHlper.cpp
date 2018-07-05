#include "stdafx.h"
#include "DbgHlper.h"
#include <stdarg.h>

void DbgPrintf(wchar_t const * const _Format, ...)
{
	va_list marker = NULL;
	va_start(marker, _Format);
	std::wstring w = FormatString(_Format, marker);
	OutputDebugString(w.c_str());
	va_end(marker);
}

std::wstring FormatString(const wchar_t *_Format, ...)
{
	std::wstring tmp;
	va_list marker = NULL;
	va_start(marker, _Format);
	size_t num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > tmp.capacity()) {
		tmp.resize(num_of_chars + 1);
	}
	vswprintf_s((wchar_t *)tmp.data(), tmp.capacity(), _Format, marker);
	va_end(marker);
	std::wstring  _str = tmp.c_str();
	return _str;
}