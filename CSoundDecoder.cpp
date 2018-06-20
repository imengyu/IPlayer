#include "stdafx.h"
#include "CSoundDecoder.h"

bool CSoundDecoder::err(wchar_t const* errmsg)
{
	wcscpy_s(lasterr, errmsg);
	return false;
}