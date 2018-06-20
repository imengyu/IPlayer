#pragma once

class CStringConv
{
public:
	CStringConv();
	~CStringConv();

	static LPWSTR a2w(LPCSTR str);
	static LPCSTR w2a(LPWSTR str);
	static void del(LPCSTR str);
	static void del(LPWSTR str);
};

