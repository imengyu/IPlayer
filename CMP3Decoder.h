#pragma once
#include "stdafx.h"
#include "CSoundDecoder.h"
#include "mpg123.h"

class CMP3Decoder :
	public CSoundDecoder
{
public:
	CMP3Decoder();
	~CMP3Decoder();


	bool Open(LPWSTR file) override;
	bool Close() override;

	int GetMusicChannelsCount()override;
	int GetMusicBitsPerSample()override;
	ULONG GetMusicSampleRate()override;
	int GetMusicBitrate();
	double GetMusicLength()override;

	DWORD GetMusicLengthSample()override;
	DWORD GetCurSample()override;
	DWORD SeekToSample(DWORD sp)override;

	double SeekToSec(double sec)override;
	size_t Read(void*  _Buffer, size_t _BufferSize) override;

	double GetCurSec();
	bool IsOpened() override;


private:
	mpg123_handle * _handle = NULL;
	off_t len, fpos = 0;
	LONG rate = 0;
	int bitrate = 0, channel = 2;
	int encoding = 0;
};