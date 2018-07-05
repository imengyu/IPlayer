#pragma once
#include "CSoundDecoder.h"
#include <stdio.h>  
#include <stdlib.h>  
#include "IPlayer.h"

class CPcmDecoder :
	public CSoundDecoder
{
public:
	CPcmDecoder(IPlayer*parent);
	~CPcmDecoder();

	bool Open(LPWSTR file) override;
	bool Close() override;


	int GetMusicChannelsCount()override;
	int GetMusicBitsPerSample()override;
	ULONG GetMusicSampleRate()override;
	double GetMusicLength()override;

	DWORD GetMusicLengthSample()override;
	DWORD GetCurSample()override;
	DWORD SeekToSample(DWORD sp)override;

	double SeekToSec(double sec)override;
	size_t Read(void*  _Buffer, size_t _BufferSize) override;

	double GetCurSec();
	bool IsOpened() override;
private:
	ULONG sample_rate = 44100;
	int channels = 2;
	int bits_per_sample = 16;

	bool require_info();
	bool required_info = false;
	bool required_info_canceld = false;
	IPlayer*parent = NULL;
	FILE * _Stream = NULL;
	int _FileSize = 0;
	double _FileSec = 0;
	double _CurSec = 0;
};
