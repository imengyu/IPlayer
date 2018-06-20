#pragma once
#include "stdafx.h"

class CSoundDecoder
{
public:
	CSoundDecoder() {}
	virtual ~CSoundDecoder() {}

	virtual bool Open(LPWSTR file) { return false; }
	virtual bool Close() { return false; }

	virtual int GetMusicChannelsCount() { return 0; }
	virtual int GetMusicBitsPerSample() { return 0; }
	virtual int GetMusicSampleRate() { return 0; }
	virtual double GetMusicLength() { return 0; }
	virtual int GetMusicBitrate() { return 0; }

	virtual double GetCurSample() { return 0; }
	virtual double SeekToSample(double sec) { return 0; }
	virtual double GetCurSec() { return 0; }
	virtual double SeekToSec(double sec) { return 0; }
	virtual int Seek(long  _Offset, int  _Origin= 0) { return 0; }
	virtual size_t Read(void*  _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount) { return 0; }

	virtual bool IsOpened() { return false; }

	virtual void Release() { delete this; }

	bool err(wchar_t const* errmsg);

	LPWSTR GetLastErr() {
		return lasterr;
	}
private:
	WCHAR lasterr[256];

};

