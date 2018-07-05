#pragma once
#include "stdafx.h"
#include <string>

std::wstring FormatString(const wchar_t *_Format, ...);
void DbgPrintf(_In_z_ _Printf_format_string_ wchar_t const* const _Format, ...);