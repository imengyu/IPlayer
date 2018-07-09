#pragma once
#include "stdafx.h"
#include "CSoundDecoder.h"
#include "faad.h"

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
	UCHAR channel; 
	
	DWORD all_samples = 0;
	DWORD cur_sample = 0;

	NeAACDecConfigurationPtr c_config = NULL;
	NeAACDecHandle hAac;
	NeAACDecFrameInfo frame_info;

	bool m_Opened = false;

	long _BufferSize = 0;
	BYTE* _PCMBuffer = NULL;
	BYTE* _AACBuffer = NULL;
	FILE*_Stream = NULL;

	void freebuffer();
};

int get_one_ADTS_frame(unsigned char ** buffer, size_t *buf_size, unsigned char * data, size_t * data_size);
int convert(NeAACDecHandle hAac, unsigned char ** bufferAAC, size_t *buf_sizeAAC, unsigned char ** bufferPCM, size_t  *buf_sizePCM, DWORD*allsamples);
