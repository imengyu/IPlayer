#pragma once
#include "stdafx.h"
#include "CSoundDecoder.h"
#include "FLAC/stream_decoder.h"
#include "CDSOutputer.h"

class CFLACDecoder :
	public CSoundDecoder
{
public:
	CFLACDecoder(bool isOgg);
	~CFLACDecoder();

	static void CFLACDecoder_FLAC__StreamDecoderErrorCallback(const FLAC__StreamDecoder * decoder, FLAC__StreamDecoderErrorStatus status, void * data);
	static FLAC__StreamDecoderWriteStatus CFLACDecoder_FLAC__StreamDecoderWriteCallback(const FLAC__StreamDecoder * decoder, const FLAC__Frame * frame, const FLAC__int32 * const buffer[], void * data);
	static void CFLACDecoder_FLAC__StreamDecoderMetadataCallback(const FLAC__StreamDecoder * decoder, const FLAC__StreamMetadata * metadata, void * data);

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

	ULONG sample_rate;
	int bit_per_sample;
	int channel;

private:
	FLAC__StreamDecoder * pDecoder;//FLAC½âÂëÆ÷¶ÔÏó
	bool opened = false, isOgg = false;
	HANDLE hEventDecorderFinish = NULL;
	ULONG bfs = 0;
	UINT sps;

	BYTE buffer_cache2[BUFFERNOTIFYSIZE];
	BYTE buffer_cache[BUFFERNOTIFYSIZE*2];
	size_t readed_bytes = 0;
	FLAC__uint64 lengthSample;
	FLAC__uint64 curPos;
	double lengthSec;
	double curosSec;
};

