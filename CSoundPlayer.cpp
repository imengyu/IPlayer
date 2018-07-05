#include "stdafx.h"
#include "CSoundPlayer.h"


CSoundPlayer::CSoundPlayer()
{
}


CSoundPlayer::~CSoundPlayer()
{
}

CSoundDecoder * CSoundPlayer::GetCurrDecoder()
{
	return nullptr;
}

LPWSTR CSoundPlayer::GetLastErr()
{
	return L"Not implemented";
}
