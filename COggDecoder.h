#pragma once
#include "CSoundDecoder.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

class COggDecoder :
	public CSoundDecoder
{
public:
	COggDecoder();
	~COggDecoder();

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

	FILE * GetStream() {
		return _Stream;
	}
private:
	FILE * _Stream = NULL;
	OggVorbis_File _OggFile;
	double m_LengthSec = 0;
	ogg_int64_t m_CurSample = 0;
	ogg_int64_t m_Samples;
	int m_Channels;//声道数
	ULONG m_SampleRate;//采样率
	int m_BitsPerSample;
	int m_BitRate;
};

