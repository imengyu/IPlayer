#pragma once
#include "CSoundDecoder.h"
#include <stdio.h>  
#include <stdlib.h>  

class CPcmDecoder :
	public CSoundDecoder
{
public:
	CPcmDecoder();
	~CPcmDecoder();

	bool Open(LPWSTR file) override;
	bool Close() override;


	int GetMusicChannelsCount()override;
	int GetMusicBitsPerSample()override;
	int GetMusicSampleRate()override;
	double GetMusicLength()override;

	double GetCurSample()override;
	double SeekToSample(double sec)override;
	double SeekToSec(double sec)override;
	size_t Read(void*  _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount) override;
	int Seek(long  _Offset, int  _Origin) override;

	double GetCurSec();
	bool IsOpened() override;
private:
	FILE * _Stream = NULL;
	int _FileSize = 0;
	double _FileSec = 0;
	double _CurSec = 0;
};

