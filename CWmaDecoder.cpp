#include "stdafx.h"
#include "CWmaDecoder.h"
#include "CriticalSection.h"

CWmaDecoder::CWmaDecoder()
{
}
CWmaDecoder::~CWmaDecoder()
{
}

bool CWmaDecoder::Open(LPWSTR file)
{
	DWORD dwOutputCount = 0, dwFormatCount = 0;
	DWORD i = 0, j = 0, dwSize = 0;
	IWMOutputMediaProps* pWMOutputMediaProps = NULL;
	WM_MEDIA_TYPE* pOutputMediaType = NULL;
	WM_MEDIA_TYPE* pFormatMediaType = NULL;
	PWAVEFORMATEX pWaveFmtEx = NULL;
	WMT_ATTR_DATATYPE dataType;
	WORD wLen = 0, wAStream = 0;
	BYTE bEnable = FALSE;
	DWORD dwValue = 0;
	BOOL bCond = FALSE;
	HRESULT hr = S_OK;
	BOOL bDescrete = TRUE;
	SHORT iSpeakers = 0;

	CoInitialize(NULL);

	CCriticalSection cs;
	cs.Enter();

	pNSSBuffer = NULL;
	pWMHeaderInfo = NULL;
	pWMSyncReader = NULL;

	hr = WMCreateSyncReader(NULL, 0, &pWMSyncReader);
	if (hr != S_OK)
	{
		cs.Leave();
		return err(L"WMCreateSyncReader error.");
	}

	// open stream
	hr = pWMSyncReader->Open(file);
	if (hr != S_OK)
	{
		pWMSyncReader->Release();
		pWMSyncReader = NULL;

		cs.Leave();
		return err(L"Bad file format.");
	}

	//get output count
	hr = pWMSyncReader->GetOutputCount(&dwOutputCount);
	if (hr != S_OK)
	{
		pWMSyncReader->Release();
		pWMSyncReader = NULL;

		cs.Leave();
		return err(L"No file Output.");
	}

	for (i = 0; i < dwOutputCount; i++)
	{
		hr = pWMSyncReader->GetOutputProps(i, &pWMOutputMediaProps);
		if (hr != S_OK) continue;

		hr = pWMOutputMediaProps->GetMediaType(NULL, &dwSize);
		if (hr != S_OK) continue;

		pOutputMediaType = (WM_MEDIA_TYPE*)malloc(dwSize);
		if (pOutputMediaType == NULL)
		{
			MessageBox(GetActiveWindow(), L"Out of memory!", L"Error", MB_OK);
			continue;
		}

		hr = pWMOutputMediaProps->GetMediaType(pOutputMediaType, &dwSize);
		if (hr != S_OK)
		{
			free(pOutputMediaType);
			pOutputMediaType = NULL;

			continue;
		}

		if (IsEqualGUID(pOutputMediaType->majortype, WMMEDIATYPE_Audio))
		{
			bEnable = (BYTE)bDescrete;
			pWMSyncReader->SetOutputSetting(i, L"EnableDiscreteOutput",
				WMT_TYPE_BOOL, (const BYTE*)&bEnable, 4);

			if (iSpeakers == 0)
			{
				pWaveFmtEx = (PWAVEFORMATEX)pOutputMediaType->pbFormat;
				dwSampleRate = pWaveFmtEx->nSamplesPerSec;
				dwChannels = pWaveFmtEx->nChannels;

				hr = pWMSyncReader->GetOutputFormatCount(i, &dwFormatCount);
				if (hr == S_OK)
				{
					for (j = 0; j < dwFormatCount; j++)
					{
						hr = pWMSyncReader->GetOutputFormat(i, j, &pWMOutputMediaProps);
						if (hr != S_OK) continue;

						hr = pWMOutputMediaProps->GetMediaType(NULL, &dwSize);
						if (hr != S_OK) continue;

						pFormatMediaType = (WM_MEDIA_TYPE*)malloc(dwSize);
						if (pFormatMediaType == NULL)
						{
							MessageBox(GetActiveWindow(), L"Out of memory!", L"Error", MB_OK);
							continue;
						}

						hr = pWMOutputMediaProps->GetMediaType(pFormatMediaType, &dwSize);
						if (hr != S_OK)
						{
							free(pFormatMediaType);
							pFormatMediaType = NULL;

							continue;
						}

						if (IsEqualGUID(pFormatMediaType->formattype, WMFORMAT_WaveFormatEx))
						{
							pWaveFmtEx = (PWAVEFORMATEX)pFormatMediaType->pbFormat;
							bCond = (pWaveFmtEx->wBitsPerSample >= dwBitsPerSample) &&
								(pWaveFmtEx->nSamplesPerSec == dwSampleRate) &&
								(pWaveFmtEx->nChannels == dwChannels);

							if (bCond)
							{
								bHasAudio = TRUE;
								dwOutput = i;
								dwBitsPerSample = pWaveFmtEx->wBitsPerSample;
								pWMSyncReader->SetOutputProps(i, pWMOutputMediaProps);
							}
						}

						free(pFormatMediaType);
						pFormatMediaType = NULL;
					}
				}
			}
			else
			{
				if (iSpeakers > 2)
				{
					dwValue = iSpeakers;
					pWMSyncReader->SetOutputSetting(i, L"SpeakerConfig",
						WMT_TYPE_DWORD, (const BYTE*)&dwValue, 4);
				}

				pWMSyncReader->GetOutputFormatCount(i, &dwFormatCount);
				for (j = 0; j< dwFormatCount; j++)
				{
					pWMSyncReader->GetOutputFormat(i, j, &pWMOutputMediaProps);
					pWMOutputMediaProps->GetMediaType(NULL, &dwSize);

					pFormatMediaType = (WM_MEDIA_TYPE*)malloc(dwSize);
					if (pFormatMediaType == NULL)
					{
						MessageBox(GetActiveWindow(), L"Out of memory!", L"Error", MB_OK);
						continue;
					}

					pWMOutputMediaProps->GetMediaType(pFormatMediaType, &dwSize);
					if (IsEqualGUID(pFormatMediaType->formattype, WMFORMAT_WaveFormatEx))
					{
						pWaveFmtEx = (PWAVEFORMATEX)pFormatMediaType->pbFormat;
						if (iSpeakers > 0)
							bCond = (pWaveFmtEx->nChannels == iSpeakers);
						else
						{
							bCond = (pWaveFmtEx->wBitsPerSample >= dwBitsPerSample) &&
								(pWaveFmtEx->nSamplesPerSec >= dwSampleRate) &&
								(pWaveFmtEx->nChannels >= dwChannels);
						}

						if (bCond)
						{
							bHasAudio = TRUE;
							dwOutput = i;
							dwChannels = pWaveFmtEx->nChannels;
							dwSampleRate = pWaveFmtEx->nSamplesPerSec;
							dwBitsPerSample = pWaveFmtEx->wBitsPerSample;
							pWMSyncReader->SetOutputProps(i, pWMOutputMediaProps);
						}
					}
				}
			}
		}

		free(pOutputMediaType);
		pOutputMediaType = NULL;

		pWMOutputMediaProps->Release();
		pWMOutputMediaProps = NULL;
	}

	if (bHasAudio)
	{
		pWMSyncReader->GetStreamNumberForOutput(dwOutput, &wStream);
		if (pWMSyncReader->SetReadStreamSamples(wStream, FALSE) == NS_E_PROTECTED_CONTENT)
			bProtected = TRUE;
		else bProtected = FALSE;

		wLen = 8;
		wAStream = 0;

		hr = pWMSyncReader->QueryInterface(IID_IWMHeaderInfo, (void**)&pWMHeaderInfo);
		if (hr == S_OK)
		{
			hr = pWMHeaderInfo->GetAttributeByName(&wAStream, L"Duration", &dataType,
				(BYTE*)&liDuration, &wLen);
			if (hr != S_OK) liDuration.LowPart = 0;
		}
	}

	cs.Leave();

	opened = 1;
	return true;
}
bool CWmaDecoder::Close()
{
	CCriticalSection cs;
	cs.Enter();

	if (pNSSBuffer != NULL)
		pNSSBuffer->Release();

	if (pWMHeaderInfo != NULL)
		pWMHeaderInfo->Release();

	if (pWMSyncReader != NULL)
	{
		pWMSyncReader->Close();
		pWMSyncReader->Release();
	}

	pNSSBuffer = NULL;
	pWMHeaderInfo = NULL;
	pWMSyncReader = NULL;

	CoUninitialize();
	cs.Leave();
	opened = 0;
	return true;
}

int CWmaDecoder::GetMusicChannelsCount()
{
	return dwChannels;
}
int CWmaDecoder::GetMusicBitsPerSample()
{
	return dwBitsPerSample;
}
ULONG CWmaDecoder::GetMusicSampleRate()
{
	return dwSampleRate;
}
int CWmaDecoder::GetMusicBitrate()
{
	if (dwBitrate = -1)
	{
		WORD len, stream;
		WMT_ATTR_DATATYPE dataType;
		len = 4;
		stream = 0;
		DWORD ret;
		if (pWMHeaderInfo->GetAttributeByName(&stream, L"Bitrate", &dataType, (BYTE*)&ret, &len) != S_OK) {	
			dwBitrate = 0;
			return 0;
		}			
		dwBitrate = ret;
	}
	return dwBitrate;
}

size_t CWmaDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	BYTE* pBuf;
	DWORD len;
	DWORD copylen;
	ULARGE_INTEGER time, duration;
	DWORD flags;
	DWORD Output;
	WORD w;
	HRESULT hr;
	size_t bytes = _BufferSize;
	size_t readedBytes = 0;
	size_t copysize;
	size_t outsize;


	while (readedBytes < _BufferSize)
	{
		if (lastoutsize > 0)
		{
			memcpy_s(lastoutbyte, _BufferSize, lastoutbyte, lastoutsize);
			lastoutsize = 0;
			readedBytes += lastoutsize;
		}
		if (pNSSBuffer != NULL)
		{
			pNSSBuffer->GetBufferAndLength((BYTE**)&pBuf, &len);
			if (dwOffset < len)
			{
				if (bytes < (len - dwOffset))
					copylen = bytes;
				else
					copylen = len - dwOffset;

				outsize = _BufferSize - readedBytes;
				if (outsize < copylen) {
					copysize = copylen - outsize;
					memcpy_s(lastoutbyte, _BufferSize, pBuf + dwOffset + copysize, outsize);
					lastoutsize += outsize;
				}
				else copysize = copylen;
				memcpy_s((BYTE*)_Buffer + readedBytes, _BufferSize, pBuf + dwOffset, copysize);
				bytes = copysize;
				dwOffset += copysize;
				readedBytes += copysize;
			}

			pNSSBuffer->Release();
			pNSSBuffer = NULL;
			dwOffset = 0;
		}

		hr = pWMSyncReader->GetNextSample(wStream, &pNSSBuffer, &time.QuadPart,
			&duration.QuadPart, &flags, &Output, &w);

		liPos.LowPart = time.LowPart;
		if (hr != S_OK)
		{
			//finished reading the file
			if (hr == NS_E_NO_MORE_SAMPLES)
				return 0;
			bytes = 0;
			return 0;
		}
	}
	return readedBytes;
}

DWORD CWmaDecoder::GetMusicLengthSample()
{
	return 0;
}

DWORD CWmaDecoder::GetCurSample()
{
	return (DWORD)((double)(liPos.LowPart / 0.1e8)*dwSampleRate);
}
DWORD CWmaDecoder::SeekToSample(DWORD sp)
{
	QWORD pos = (QWORD)(0.1E8 *  (QWORD)(sp) / dwSampleRate);
	liPos.QuadPart = pos;
	pWMSyncReader->SetRange((QWORD)(pos), 0);
	return sp;
}
double CWmaDecoder::GetMusicLength()
{
	return (double)(liDuration.LowPart / 0.1e8);
}
double CWmaDecoder::SeekToSec(double sec)
{
	QWORD pos = (QWORD)(0.1E8 *  (QWORD)(sec));
	liPos.QuadPart = pos;
	pWMSyncReader->SetRange((QWORD)(pos), 0);
	return sec;
}
double CWmaDecoder::GetCurSec()
{
	return (double)(liPos.LowPart / 0.1e8);
}
bool CWmaDecoder::IsOpened()
{
	return opened;
}