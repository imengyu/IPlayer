#pragma once
#include "CSoundDecoder.h"

class CAacDecoder :
	public CSoundDecoder
{
public:
	CAacDecoder();
	~CAacDecoder();

	bool Open(LPWSTR file) override;
	bool Close() override;


	int GetMusicChannelsCount()override;
	int GetMusicBitsPerSample()override;
	DWORD GetMusicSampleRate()override;
	double GetMusicLength()override;

	double SeekToSec(double sec)override;
	double GetCurSec();

	DWORD GetMusicLengthSample()override;
	DWORD GetCurSample()override;
	DWORD SeekToSample(DWORD sp)override;

	size_t Read(void*  _Buffer, size_t _BufferSize) override;


	bool IsOpened() override;

private:
	ULONG sample_rate;
	int bit_per_sample;
	int channel;


};

