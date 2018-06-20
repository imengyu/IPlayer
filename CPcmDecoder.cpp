#include "stdafx.h"
#include "CPcmDecoder.h"


CPcmDecoder::CPcmDecoder()
{
}
CPcmDecoder::~CPcmDecoder()
{
	if (_Stream)
		Close();
}

bool CPcmDecoder::Open(LPWSTR file)
{
    _wfopen_s(&_Stream, file, L"rb");
	if (_Stream) {
		fseek(_Stream, 0L, SEEK_END);
		_FileSize = ftell(_Stream);
		fseek(_Stream, 0, SEEK_SET);
		_FileSec = (double)_FileSize / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);
		return true;
	}
	else err(L"Failed open file.");
	return false;
}
bool CPcmDecoder::Close()
{
	fclose(_Stream);
	_Stream = NULL;
	return true;
}

int CPcmDecoder::GetMusicChannelsCount()
{
	return 2;
}
int CPcmDecoder::GetMusicBitsPerSample()
{
	return 16;
}
int CPcmDecoder::GetMusicSampleRate()
{
	return 44100;
}
double CPcmDecoder::GetMusicLength()
{
	return _FileSec;
}

double CPcmDecoder::GetCurSample()
{
	long cur = ftell(_Stream);
	_CurSec = (double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);
	return  (double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample()) / 8);;
}

double CPcmDecoder::SeekToSample(double sec)
{
	_CurSec = sec/ GetMusicSampleRate();
	SeekToSec(_CurSec);
	return 0.0;
}

double CPcmDecoder::SeekToSec(double sec)
{
	_CurSec = sec;
	return static_cast<double>(fseek(_Stream, static_cast<LONG>((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8*sec), SEEK_SET));
}

size_t CPcmDecoder::Read(void * _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount)
{
	if (!_Stream)return -1;
	return fread_s(_Buffer, _BufferSize, _ElementSize, _ElementCount, _Stream);
}
int CPcmDecoder::Seek(long  _Offset,  int   _Origin)
{
	if (!_Stream)return -1;
	return fseek(_Stream, _Offset, _Origin);
}
double CPcmDecoder::GetCurSec()
{
	long cur = ftell(_Stream);
	_CurSec = (double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate())/8);
	return _CurSec;
}

bool CPcmDecoder::IsOpened()
{
	return _Stream != NULL;
}
