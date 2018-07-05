#include "CAacDecoder.h"
#include "faac/include/faac.h"

CAacDecoder::CAacDecoder()
{
}
CAacDecoder::~CAacDecoder()
{
}

bool CAacDecoder::Open(LPWSTR file)
{
	return false;
}

bool CAacDecoder::Close()
{
	return false;
}

int CAacDecoder::GetMusicChannelsCount()
{
	return 0;
}

int CAacDecoder::GetMusicBitsPerSample()
{
	return 0;
}

DWORD CAacDecoder::GetMusicSampleRate()
{
	return 0;
}

double CAacDecoder::GetMusicLength()
{
	return 0.0;
}

double CAacDecoder::SeekToSec(double sec)
{
	return 0.0;
}

double CAacDecoder::GetCurSec()
{
	return 0.0;
}

DWORD CAacDecoder::GetMusicLengthSample()
{
	return 0;
}

DWORD CAacDecoder::GetCurSample()
{
	return 0;
}

DWORD CAacDecoder::SeekToSample(DWORD sp)
{
	return 0;
}

size_t CAacDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	return size_t();
}

bool CAacDecoder::IsOpened()
{
	return false;
}
