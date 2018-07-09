#include "stdafx.h"
#include "CSoundDecoder.h"
#include "CStringConv.h"

bool CSoundDecoder::err(wchar_t const* errmsg)
{
	wcscpy_s(lasterr, errmsg);
	return false;
}

bool CSoundDecoder::err(char const * errmsg)
{
	LPWSTR c = CStringConv::a2w(errmsg);
	wcscpy_s(lasterr, c);
	CStringConv::del(c);
	return false;
}
