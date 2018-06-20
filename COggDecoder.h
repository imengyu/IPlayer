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
	int GetMusicSampleRate()override;
	int GetMusicBitrate();
	double GetMusicLength()override;

	double GetCurSample()override;
	double SeekToSample(double sec)override;
	double SeekToSec(double sec)override;
	size_t Read(void*  _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount) override;
	int Seek(long  _Offset, int  _Origin) override;

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
	int m_SampleRate;//采样率
	int m_BitsPerSample;
	int m_BitRate;
};

