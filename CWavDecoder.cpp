#include "stdafx.h"
#include "CWavDecoder.h"


CWavDecoder::CWavDecoder()
{
	memset(&m_pwfx, 0, sizeof(m_pwfx));
}
CWavDecoder::~CWavDecoder()
{
	if (IsOpened())
		Close();
}

bool CWavDecoder::Open(LPWSTR file)
{
	ZeroMemory(&wavInfo, sizeof(wavInfo));
	hStream = mmioOpen(file, &wavInfo, MMIO_READ);
	if (!hStream) {
		err(L"Failed open file.");
		return false;
	}

	PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.  
	memset(&ckIn, 0, sizeof(ckIn));

	if ((0 != mmioDescend(hStream, &m_ckRiff, NULL, 0)))
		return false;
	// Check to make sure this is a valid wave file  
	if ((m_ckRiff.ckid != FOURCC_RIFF) ||
		(m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		return false;

	// Search the input file for for the 'fmt ' chunk.  
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (0 != mmioDescend(hStream, &ckIn, &m_ckRiff, MMIO_FINDCHUNK))
		return false;

	// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;  
	// if there are extra parameters at the end, we'll ignore them  
	if (ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT))
		return false;

	// Read the 'fmt ' chunk into <pcmWaveFormat>.  
	if (mmioRead(hStream, (HPSTR)&pcmWaveFormat,
		sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
		return false;

	// Allocate the waveformatex, but if its not pcm format, read the next  
	// word, and thats how many extra bytes to allocate.  
	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
	{
		// Copy the bytes from the pcm structure to the waveformatex structure  
		memcpy(&m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx.cbSize = 0;
	}
	else
	{
		// Read in length of extra bytes.  
		WORD cbExtraBytes = 0L;
		if (mmioRead(hStream, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD))
			return false;

		// Copy the bytes from the pcm structure to the waveformatex structure  
		memcpy(&m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx.cbSize = cbExtraBytes;

		// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.  
		if (mmioRead(hStream, (CHAR*)(((BYTE*)&(m_pwfx.cbSize)) + sizeof(WORD)),
			cbExtraBytes) != cbExtraBytes)
			return false;
	}

	// Ascend the input file out of the 'fmt ' chunk.  
	if (0 != mmioAscend(hStream, &ckIn, 0))
		return false;

	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (0 != mmioDescend(hStream, &ckIn, &m_ckRiff, MMIO_FINDCHUNK))
		return false;

	_FileSize = m_ckRiff.cksize;
	_DataOffset = m_ckRiff.dwDataOffset;
	_BitPerSample = ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);;
	_FileSec = (double)_FileSize / _BitPerSample;
	_CurSec = 0;
	cur = 0;

	return true;
}
bool CWavDecoder::Close()
{
	if (0 != mmioAscend(hStream, &ckIn, 0))
		return false;
	if (!hStream)return false;
	return mmioClose(hStream, 0)==0;
}
int CWavDecoder::GetMusicChannelsCount()
{
	return m_pwfx.nChannels;
}
int CWavDecoder::GetMusicBitsPerSample()
{
	return m_pwfx.wBitsPerSample;
}
ULONG CWavDecoder::GetMusicSampleRate()
{
	return m_pwfx.nSamplesPerSec;
}
double CWavDecoder::GetMusicLength()
{
	return _FileSec;
}
DWORD CWavDecoder::GetMusicLengthSample()
{
	return 0;
}
DWORD CWavDecoder::GetCurSample()
{
	_CurSec = static_cast<DWORD>((double)cur / (double)_BitPerSample);
	return static_cast<DWORD>(_CurSec* GetMusicSampleRate());
}
DWORD CWavDecoder::SeekToSample(DWORD sec)
{
	_CurSec = sec / (double)GetMusicSampleRate();
	SeekToSec(_CurSec);
	return 0;
}
double CWavDecoder::SeekToSec(double sec)
{
	_CurSec = sec;
	return (double)Seek((LONG)(_BitPerSample*sec), SEEK_SET);
}
size_t CWavDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	if (hStream) {
		long l = mmioRead(hStream, (HPSTR)_Buffer, _BufferSize);
		cur += l;
		return l;
	}
	return 0;
}
int CWavDecoder::Seek(long _Offset, int _Origin)
{
	if (hStream) {
		if (_Origin == SEEK_SET)cur = _Offset;
		else if (_Origin == SEEK_CUR)cur += _Offset;
		return mmioSeek(hStream, _Offset, _Origin);
	}
	return 0;
}
double CWavDecoder::GetCurSec()
{
	_CurSec = (double)cur / (double)_BitPerSample;
	return _CurSec;
}
bool CWavDecoder::IsOpened()
{
	return  hStream != NULL;;
}
