#include "stdafx.h"
#include "COggDecoder.h"

COggDecoder*currectOggDecoder = NULL;

COggDecoder::COggDecoder()
{
	currectOggDecoder = this;
}
COggDecoder::~COggDecoder()
{
	if (IsOpened())
		Close();
	currectOggDecoder = NULL;
}

size_t vorbisRead(void *buffer, size_t size, size_t amount, void *file)
{
	size_t result = 0;
	result = fread_s(buffer, size*amount, 1, amount, (FILE*)file);
	return result;
}
int vorbisSeek(void *file, ogg_int64_t newpos, int set)
{
	return (int)fseek((FILE*)file,static_cast<long>(newpos), set);;
}
int vorbisClose(void *file)
{
	return 1;
}
long vorbisTell(void *file)
{
	return ftell((FILE*)file);
}

bool COggDecoder::Open(LPWSTR file)
{
	_wfopen_s(&_Stream, file, L"rb");
	fseek(_Stream, 0, SEEK_SET);
	if (_Stream)
	{
		ov_callbacks callbacks = {
			(size_t(*)(void *, size_t, size_t, void *)) vorbisRead,
			(int(*)(void *, ogg_int64_t, int)) vorbisSeek,
			(int(*)(void *)) vorbisClose,
			(long(*)(void *)) vorbisTell
		};

		//ov_open(_Stream, &_OggFile, NULL, 0);

		if (ov_open_callbacks((void *)_Stream, &_OggFile, NULL, 0, callbacks) < 0)
		{		
			wprintf(L"Input does not appear to be an Ogg bitstream.");
			err(L"Input does not appear to be an Ogg bitstream.");
			Close();
			return false;
		}
		if (ov_streams(&_OggFile) != 1)
		{
			TCHAR BUFFER[512];
			wsprintf(BUFFER, L"Can\'t deal with multiple streams yet. Streams:%d",
				ov_streams(&_OggFile));
			wprintf(BUFFER);
			err(BUFFER);
			Close();
			return false;
		}

		vorbis_info *pInfo = NULL;
		pInfo = ov_info(&_OggFile, -1);
		if (pInfo == NULL)
		{
			wprintf(L"Unable to get ogg info.");
			err(L"Unable to get ogg info.");
			Close();
			return false;
		}
		if (pInfo->channels > 2)
		{
			TCHAR BUFFER[512];
			wsprintf(BUFFER, L"Can\'t deal with more than 2 channels yet. Channels:%d" , pInfo->channels);
			wprintf(BUFFER);
			err(BUFFER);
			Close();
			return false;
		}

		m_SampleRate = static_cast<ULONG>(pInfo->rate);
		m_Channels = pInfo->channels; 
		m_BitsPerSample = 16;
		m_BitRate = pInfo->bitrate_nominal;

		m_Samples = ov_pcm_total(&_OggFile, -1);
		m_LengthSec = static_cast<double>(m_Samples) / (double)m_SampleRate;

		return true;
	}
	return false;
}
bool COggDecoder::Close()
{
	ov_clear(&_OggFile);
	if (_Stream)
		fclose(_Stream);
	_Stream = NULL;
	return false;
}

int COggDecoder::GetMusicChannelsCount()
{
	return m_Channels;
}
int COggDecoder::GetMusicBitsPerSample()
{
	return m_BitsPerSample;
}
ULONG COggDecoder::GetMusicSampleRate()
{
	return m_SampleRate;
}
int COggDecoder::GetMusicBitrate()
{
	return m_BitRate;
}
DWORD COggDecoder::GetMusicLengthSample()
{
	return static_cast<DWORD>(m_Samples);
}

double COggDecoder::GetMusicLength()
{
	return m_LengthSec;
}
DWORD COggDecoder::GetCurSample()
{
	m_CurSample = ov_pcm_tell(&_OggFile);
	return static_cast<DWORD>(m_CurSample);
}
DWORD COggDecoder::SeekToSample(DWORD sp)
{
	return ov_pcm_seek(&_OggFile, static_cast<ogg_int64_t>(sp));
}
double COggDecoder::SeekToSec(double sec)
{
	return ov_pcm_seek(&_OggFile, static_cast<ogg_int64_t>(sec*m_SampleRate));
}

size_t COggDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	size_t result = 0;
	BOOL CONTINUE = TRUE;
	DWORD bytes_read = 0;

	while (bytes_read < _BufferSize)
	{
		result = ov_read(&_OggFile, (char *)_Buffer + bytes_read,
			_BufferSize - bytes_read, 0, 2, 1, NULL);

		if (result == 0)
		{
			CONTINUE = FALSE;
			break;
		}
		else if (result < 0)
		{
			OutputDebugString(L"Hole in OGG/VORBIS datastream, ignoring...");
		}

		else
		{
			bytes_read += result;
		}
	}
	result = bytes_read;
	return result;
}
double COggDecoder::GetCurSec()
{
	m_CurSample = ov_pcm_tell(&_OggFile);
	return m_CurSample / (double)m_SampleRate;
}
bool COggDecoder::IsOpened()
{
	return _Stream != NULL;
}
