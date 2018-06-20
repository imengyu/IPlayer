#include "stdafx.h"
#include "CMP3Decoder.h"
#include "CStringConv.h"

CMP3Decoder::CMP3Decoder()
{

}
CMP3Decoder::~CMP3Decoder()
{

}

bool CMP3Decoder::Open(LPWSTR file)
{
	int ret = -1;
	_handle = mpg123_new(NULL, &ret);
	if (_handle == NULL || ret != MPG123_OK)
		return false;

	LPCSTR str = CStringConv::w2a(file);
	if (mpg123_open(_handle, str) != MPG123_OK) {
		CStringConv::del(str);
		err(L"Open file error.");
		return false;
	}
	CStringConv::del(str);

	if (mpg123_getformat(_handle, &rate, &channel, &encoding) != MPG123_OK) {
		err(L"Bad file format.");
		return false;
	}
	if ((encoding & MPG123_ENC_16) == MPG123_ENC_16)
		encoding = 16;
	else if ((encoding & MPG123_ENC_32) == MPG123_ENC_32)
		encoding = 32;
	else
		encoding = 8;
	len = mpg123_length(_handle);
	if (len <= 0) {
		err(L"Bad file.");
		return false;
	}
	return true;
}
bool CMP3Decoder::Close()
{
	if (_handle != NULL)
	{
		mpg123_close(_handle);
		mpg123_delete(_handle);
		_handle = NULL;
	}
	return true;
}
int CMP3Decoder::GetMusicChannelsCount()
{
	return channel;
}
int CMP3Decoder::GetMusicBitsPerSample()
{
	return encoding;
}
int CMP3Decoder::GetMusicSampleRate()
{
	return rate;
}
int CMP3Decoder::GetMusicBitrate()
{
	return bitrate;
}
double CMP3Decoder::GetMusicLength()
{
	return len / rate;
}
double CMP3Decoder::GetCurSample()
{
	off_t o = mpg123_tell(_handle);
	return o;
}
double CMP3Decoder::SeekToSample(double sec)
{
	return (mpg123_seek(_handle, static_cast<off_t>(sec), SEEK_SET));
}
double CMP3Decoder::SeekToSec(double sec)
{
	return Seek(static_cast<long>(sec*rate), SEEK_SET);
}
size_t CMP3Decoder::Read(void * _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount)
{
	size_t t;
	mpg123_read(_handle, (UCHAR*)_Buffer, _ElementCount*_ElementSize, &t);
	return t;
}
int CMP3Decoder::Seek(long _Offset, int _Origin)
{
	return mpg123_seek(_handle, _Offset, _Origin);
}
double CMP3Decoder::GetCurSec()
{
	off_t o = mpg123_tell(_handle);
	fpos = o / rate;
	return fpos;
}
bool CMP3Decoder::IsOpened()
{
	return _handle !=NULL;
}
