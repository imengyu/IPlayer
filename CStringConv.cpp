#include "stdafx.h"
#include "CStringConv.h"


CStringConv::CStringConv()
{
}


CStringConv::~CStringConv()
{
}

LPWSTR CStringConv::a2w(LPCSTR szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

LPCSTR CStringConv::w2a(LPWSTR szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;
}

void CStringConv::del(LPCSTR str)
{
	delete str;
}
void CStringConv::del(LPWSTR str)
{
	delete str;
}