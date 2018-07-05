#include "stdafx.h"
#include "IPlayer.h"
#include <iostream>
#include <time.h>


#include "CPcmDecoder.h"
#include "CWavDecoder.h"
#include "CMP3Decoder.h"
#include "COggDecoder.h"
#include "CMidiDecoder.h"
#include "CWmaDecoder.h"
#include "CFLACDecoder.h"

#include "CDSOutputer.h"
#include <Digitalv.h>

IPlayer::IPlayer(HWND hWnd)
{
	hostHWnd = hWnd;
	m_outputer = new CDSOutputer(this);
	m_outputer->SetOnCopyDataCallback(OnCopyData);
}
IPlayer::~IPlayer()
{
	if (m_playerStatus == Playing)
		Stop();

	Close();
	if(m_outputer)delete m_outputer;
}

bool IPlayer::Open(LPWSTR file)
{
	m_NextStop = FALSE;
	if (!m_LastDestroyed)
		Close();
	TStreamFormat thisFileFormat = GetFileFormat(file);
	if (thisFileFormat == TStreamFormat::sfUnknown)return false;
	if (m_openedFileFormat != thisFileFormat) {
		if (m_decoder)
		{
			if (m_decoder->IsOpened())
				m_decoder->Close();
			m_decoder->Release();
			m_decoder = NULL;
		}
	}
	if (m_decoder == NULL)
		m_decoder = CreateDecoderWithFormat(thisFileFormat);
	if (m_decoder != NULL) {
		m_openedFileFormat = thisFileFormat;
		if (m_decoder->Open(file)) {
			m_decoder->SeekToSec(0);
			m_playerStatus = TPlayerStatus::Opened;

			int i1 = m_decoder->GetMusicSampleRate(), i2 = m_decoder->GetMusicChannelsCount(), i3 = m_decoder->GetMusicBitsPerSample();
			if (i1 == -1 || i2 == -1 || i3 == -1) {
				Close();
				return err(L"User canceled play.");
			}
			m_outputer->Create(hostHWnd, i1, i2, i3);

			if (playerVol != m_outputer->GetVol())m_outputer->SetVol(playerVol);

			time_t t;
			tm p;
			t = static_cast<long long>(m_decoder->GetMusicLength());
			gmtime_s(&p, &t);
			memset(isplayingAllTime, 0, sizeof(isplayingAllTime));
			wcsftime(isplayingAllTime, sizeof(isplayingAllTime) / sizeof(wchar_t), L"%M:%S", &p);
			m_LastDestroyed = FALSE;

			UpdatePos();

			return true;
		}
		else err(m_decoder->GetLastErr());
	}
	else err(L"Failed create decoder!\nBad file format!");
	return false;
}
bool IPlayer::Close()
{
	if (IsOpened()) {
		if (m_outputer != NULL) {
			m_outputer->Destroy();
		}
		if (m_decoder != NULL) {
			m_decoder->Close();
			delete m_decoder;
			m_decoder = NULL;
		}		
		m_playerStatus = NotOpen;
		m_LastDestroyed = TRUE;
		return true;
	}
	else err(L"Not open.");
	return false;
}

bool IPlayer::PausePlay()
{
	if (m_playingMidi)
	{
		if (IsOpened())
		{
			DWORD music_playstyle = ((CMidiDecoder*)m_decoder)->GetStatus();
			switch (music_playstyle)
			{
			case 525:
				return Play();
			case 526:
				return Pause();
			default:
				return Restart();
			}
		}
	}
	else
	{
		if (m_playerStatus == Playing)
			return Pause();
		else if (m_playerStatus == Paused)
			return Play();
		else  if (m_playerStatus == Opened)
			return Play();
	}
	return false;
}
bool IPlayer::Play()
{
	if (IsOpened())
	{
		if (m_playingMidi)
		{
			if (m_playerStatus != TPlayerStatus::Playing)
			{
				MCI_PLAY_PARMS PlayParms;
				mciSendCommand(((CMidiDecoder*)m_decoder)->GetMidiId(), MCI_PLAY, 0,
					(DWORD)(LPVOID)&PlayParms);

				m_playerStatus = TPlayerStatus::Playing;
				return true;
			}
		}
		else {
			if (m_playerStatus != TPlayerStatus::Playing)
			{
				if (!m_outputer->IsOutPuting())m_outputer->StartOutPut();
				m_playerStatus = TPlayerStatus::Playing;
				return true;
			}
			else return true;
		}
	}
	return false;
}
bool IPlayer::Pause()
{
	if (IsOpened())
	{
		if (m_playingMidi)
		{
			if (m_playerStatus != TPlayerStatus::Paused)
			{
				MCI_PLAY_PARMS PlayParms;
				mciSendCommand(((CMidiDecoder*)m_decoder)->GetMidiId(), MCI_PAUSE, 0,
					(DWORD)(LPVOID)&PlayParms);
				m_playerStatus = TPlayerStatus::Paused;
				return true;
			}		
		}
		else {
			if (m_playerStatus != TPlayerStatus::Paused)
			{
				if (m_outputer->IsOutPuting())m_outputer->EndOutPut();
				m_playerStatus = TPlayerStatus::Paused;
				return true;
			}
			else return true;
		}
	}
	return false;
}
bool IPlayer::Stop()
{
	if (IsOpened())
	{
		if (m_playingMidi)
		{
			if (m_playerStatus != TPlayerStatus::Opened)
			{
				MCI_PLAY_PARMS PlayParms;
				mciSendCommand(((CMidiDecoder*)m_decoder)->GetMidiId(), MCI_STOP, 0,
					(DWORD)(LPVOID)&PlayParms);

				m_playerStatus = TPlayerStatus::Opened;
				return true;
			}
		}
		else {
			if (m_playerStatus != TPlayerStatus::Opened)
			{
				if (m_outputer->IsOutPuting())
					m_outputer->EndOutPut();

				m_playerStatus = TPlayerStatus::Opened;
				return true;
			}
			else return true;
		}
	}
	return false;
}
bool IPlayer::Restart()
{
	if (IsOpened())
	{
		if (m_playingMidi)
		{
			MCI_PLAY_PARMS PlayParms;
			PlayParms.dwFrom = 0;
			mciSendCommand(((CMidiDecoder*)m_decoder)->GetMidiId(), MCI_PLAY,
				MCI_FROM, (DWORD)(LPVOID)
				&PlayParms);
			m_playerStatus = TPlayerStatus::Playing;
		}
		else {
			m_decoder->SeekToSec(0);
			return Play();
		}
	}
	return false;
}

bool IPlayer::IsOpened()
{
	if (m_decoder)return m_decoder->IsOpened();
	return false;
}

TStreamFormat IPlayer::GetFormat()
{
	return m_openedFileFormat;
}

IPlayer *currentFadePlayer = NULL;
bool currentFadeIn = false;
int currentFadeVal = 0;
double currentFadeOff = 5.0;
int endFadeVol = 0;

WCHAR teststr[128];

LPWSTR IPlayer::GetMusicPosTimeString() {
	return isplayingPosTime;
}
LPWSTR IPlayer::GetMusicLenTimeString() {
	return isplayingAllTime;
}
LPWSTR IPlayer::GetMusicTimeString() {
	return isplayingTime;
}
TPlayerStatus IPlayer::GetPlayerStatus()
{
	return m_playerStatus;
}
DWORD IPlayer::GetMusicPosSample()
{
	if (m_decoder&&m_decoder->IsOpened())
		return m_decoder->GetCurSample();
	return -1;
}
DWORD IPlayer::SetMusicPosSample(DWORD sample)
{
	if (IsOpened()) {
		if (sample != m_decoder->GetCurSample()) {
			m_decoder->SeekToSample(sample);
			if (!m_playingMidi) m_outputer->ResetOutPut();
		}
	}
	return sample;
}
double IPlayer::GetMusicPos()
{
	if (m_decoder&&m_decoder->IsOpened())
	{
		double c = 0;
		//if (m_playingMidi) 
		c = m_decoder->GetCurSec();
		//else c = m_outputer->GetCurrPos();
		time_t t;
		tm p;
		t = static_cast<long long>(c);
		gmtime_s(&p, &t);
		memset(isplayingTime, sizeof(isplayingTime), 0);
		wcsftime(isplayingTime, sizeof(isplayingAllTime) / sizeof(wchar_t), L"%M:%S", &p);
		wcscpy_s(isplayingPosTime, isplayingTime);
		wcscat_s(isplayingTime, L"/");
		wcscat_s(isplayingTime, isplayingAllTime);

		//swprintf_s(teststr, L"pos : %f depos : %f outposdl s : %d + outposc s : %d = outpos s : %d depos s : %d \n", 
		//	c, m_decoder->GetCurSec(), ((CDSOutputer*)m_outputer)->cur_decorder_pos, 
		//	m_outputer->GetOutPutingPosSample(),
		//	m_outputer->GetCurrPosSample(), m_decoder->GetCurSample());
		//OutputDebugString(teststr);

		return c;
	}
	return -1;
}
double IPlayer::GetMusicLength()
{
	if (m_decoder&&m_decoder->IsOpened())
		return m_decoder->GetMusicLength();
	else return -1;
}
DWORD  IPlayer::GetMusicLengthSample()
{
	if (m_decoder&&m_decoder->IsOpened())
		return m_decoder->GetMusicLengthSample();
	else return -1;
}
bool IPlayer::SetMusicPos(double sec)
{
	if (IsOpened()) {
		if (sec != m_decoder->GetCurSec()) {
			m_decoder->SeekToSec(sec);
			if (!m_playingMidi) m_outputer->ResetOutPut();
		}
	}
	return false;
}
void IPlayer::FadeOut(int sec, int from, int to)
{
	currentFadeOff = (from - to)*0.1;
	currentFadePlayer = this;
	SetTimer(hostHWnd, 26, static_cast<UINT>(sec*0.1*1000), (TIMERPROC)OnFadeTime);
	currentFadeIn = false;
	currentFadeVal = from;
	endFadeVol = to;
}
void IPlayer::FadeIn(int sec, int from, int to)
{
	currentFadeOff = (to - from)*0.1;
	currentFadePlayer = this;
	SetTimer(hostHWnd, 26, static_cast<UINT>(sec*0.1 * 1000), (TIMERPROC)OnFadeTime);
	currentFadeIn = true;
	currentFadeVal = from;
	endFadeVol = to;
}
void IPlayer::SetDefOutPutSettings(ULONG sample_rate, int channels, int bits_per_sample)
{
	def_sample_rate = sample_rate;
	def_channels = channels;
	def_bits_per_sample = bits_per_sample;
	def_out_seted = true;
}
int IPlayer::GetPlayerVolume()
{
	if (m_playingMidi)
		return playerVol;
	return m_outputer->GetVol();
}
void IPlayer::SetPlayerVolume(int vol)
{
	playerVol = vol;
	if (m_playingMidi)
	{
		MCI_DGV_SETAUDIO_PARMS mciSetAudioPara;
		mciSetAudioPara.dwItem = MCI_DGV_SETAUDIO_VOLUME;
		mciSetAudioPara.dwValue = vol * 10;
		mciSendCommand(((CMidiDecoder*)m_decoder)->GetMidiId(), MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)(LPVOID)&mciSetAudioPara);
	}
	else m_outputer->SetVol(vol);
}
bool IPlayer::IsPlayingMidi() {
	return m_playingMidi;
}

TStreamFormat IPlayer::GetFileFormat(const wchar_t * pchFileName)
{
	err(L"");
	if (pchFileName == 0)
	{
		err(L"FUNCTION_PARAMETERS");
		return sfUnknown;
	}
	// check special formats
	/*if (wcsncmp(pchFileName, L"wavein://", 9) == 0)
	{
		// delete custom mixer line id
		if (c_user_mixer_line)
		{
			free(c_user_mixer_line);
			c_user_mixer_line = NULL;
		}
		TStreamFormat format = sfUnknown;
		char fn[MAX_PATH + 1];
		fn[0] = 0;
		if (WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, pchFileName, -1, fn, MAX_PATH, NULL, NULL) == 0)
		{
			err(L"UNKNOWN_ERROR");
			return sfUnknown;
		}
		fn[MAX_PATH] = 0;
		// parse input line

		char *start = fn + 9;
		char *name = NULL;
		char *value = NULL;
		int found = 0;
		c_user_mixer_line_id = 0;//WAVE_IN_NUM;
		c_user_mixer_volume = 200; // set to 200 because this value will not change volume in WaveIn
		char *context;
		char *token = strtok_s(start, ";", &context);
		while (token)
		{
			name = token;
			char *end = strchr(token, '=');
			if (end)
			{
				*end = 0;
				value = end + 1;

				// parse source
				if (stricmp(name, "src") == 0)
				{
					unsigned int i;
					for (i = 0; i < WAVE_IN_NUM; i++)
					{
						if (stricmp(value, g_wavein_id[i].string_id) == 0)
						{
							c_user_mixer_line_id = i;
							found = 1;
							break;

						}
					}
				}
				else if (_stricmp(name, "name") == 0) // parse user name
				{
					if (*value == '(')
					{
						value = value + 1;
						char *end1 = strchr(value, ')');
						if (end1)
						{
							*end1 = 0;
							c_user_mixer_line = _strdup(value);
						}
					}

				}
				else if (_stricmp(name, "volume") == 0) // parse user name
				{
					c_user_mixer_volume = atoi(value);

				}
			}
			token = strtok_s(NULL, ";", &context);
		}
		if (found)
		{
			format = sfWaveIn;
		}
		else
		{
			if (c_user_mixer_line_id)
			{
				format = sfWaveIn;
			}
		}

		if (format == sfUnknown)
			err(L"UNKNOWN_FILE_FORMAT");
		return format;
	}*/

	// parse filename and try to get file format using filename extension
	wchar_t *ext = wcsrchr((wchar_t*)pchFileName, L'.');
	if (ext)
	{
		ext++;

		if (_wcsicmp(ext, L"mp3") == 0)
			return sfMp3;
		if (_wcsicmp(ext, L"mp2") == 0)
			return sfMp3;
		if (_wcsicmp(ext, L"mp1") == 0)
			return sfMp3;
		if (_wcsicmp(ext, L"wma") == 0)
			return sfWma;
		if (_wcsicmp(ext, L"aac") == 0)
			return sfAacADTS;
		if (_wcsicmp(ext, L"ogg") == 0)
			return sfOgg;
		if (_wcsicmp(ext, L"wav") == 0)
			return sfWav;
		if (_wcsicmp(ext, L"flac") == 0)
			return sfFLAC;
		if (_wcsicmp(ext, L"oga") == 0)
			return sfFLACOgg;
		if (_wcsicmp(ext, L"ac3") == 0)
			return sfAC3;
		if (_wcsicmp(ext, L"pcm") == 0)
			return sfPCM;
		if (_wcsicmp(ext, L"midi") == 0)
			return sfMidi;
		if (_wcsicmp(ext, L"mid") == 0)
			return sfMidi;	
	}

	// failed to determine file format using filename extension

	// check if file has ogg mark

	// open file
	HANDLE hFile = CreateFileW(pchFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}

	// get file size
	unsigned int nStreamSize = GetFileSize(hFile, NULL);
	if (nStreamSize == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}

	char tmp[4];
	DWORD dwRead;
	// ====================================================================
	//	CHECK IG THIS IS FLAC OGG FILE
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}
	// check if file can be ogg, check first 4 bytes for OggS identification.
	if (ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if (strncmp(tmp, "OggS", 4) == 0)
		{
			// this can be ogg file, check if this is FLAC OGG
			if (SetFilePointer(hFile, 29, NULL, FILE_BEGIN) != 0xFFFFFFFF)
			{
				if (ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
				{
					if (strncmp(tmp, "FLAC", 4) == 0)
					{
						CloseHandle(hFile);
						return sfFLACOgg;
					}
				}
			}
		}
	}

	// ====================================================================
	//	CHECK IF THIS IS OGG FILE
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}
	// check if file can be ogg, check first 4 bytes for OggS identification.
	if (ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if (strncmp(tmp, "OggS", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfOgg;
		}
	}
	// ====================================================================
	//	CHECK IG THIS IS WAVE FILE WITH RIFF HEADER
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}
	// check if file can be ogg, check first 4 bytes for OggS identification.
	if (ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if (strncmp(tmp, "RIFF", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfWav;
		}
	}
	// ====================================================================
	//	CHECK IG THIS IS FLAC FILE
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(L"FILE_OPEN_ERROR");
		return sfUnknown;
	}
	// check if file can be ogg, check first 4 bytes for OggS identification.
	if (ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if (strncmp(tmp, "fLaC", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfFLAC;
		}
	}
	// =================================================
	CloseHandle(hFile);
	err(L"UNKNOWN_FILE_FORMAT");
	return sfUnknown;
}

CSoundDecoder * IPlayer::CreateDecoderWithFormat(TStreamFormat f)
{
	switch (f)
	{
	case sfMp3:
		m_playingMidi = FALSE;
		return new CMP3Decoder();
	case sfOgg:
		m_playingMidi = FALSE;
		return new COggDecoder();
	case sfWav:
		m_playingMidi = FALSE;
		return new CWavDecoder();
	case sfPCM:
		m_playingMidi = FALSE;
		return new CPcmDecoder(this);
	case sfMidi:
		m_playingMidi = TRUE;
		return new CMidiDecoder(this);
	case sfWma:
		return new CWmaDecoder();
	case sfFLAC:
		return new CFLACDecoder(false);
	case sfFLACOgg:
		return new CFLACDecoder(true);
	default:
		err(L"Unsupport file format.");
		break;
	}
	return nullptr;
}

void IPlayer::SetEndStatus() {
	m_playerStatus = TPlayerStatus::PlayEnd;
}
DWORD IPlayer::UpdatePos() {
	DWORD d = m_decoder->GetCurSample();
	return d;
}

bool IPlayer::OnCopyData(CSoundPlayer*instance, LPVOID buf, DWORD buf_len)
{
	if (!instance->IsPlayingMidi())
	{
		if (((IPlayer*)instance)->m_NextStop)
		{
			((IPlayer*)instance)->m_NextStop = FALSE;
			((IPlayer*)instance)->m_playerStatus = PlayEnd;
			((IPlayer*)instance)->m_outputer->EndOutPut();
			return false;
		}
		else if (((IPlayer*)instance)->m_decoder->Read(buf, buf_len) < buf_len)
		{
			((IPlayer*)instance)->m_NextStop = TRUE;
			return true;
		}
		return true;
	}	
	return false;
}

VOID CALLBACK IPlayer::OnFadeTime(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	if (!currentFadePlayer) { KillTimer(hwnd, 26); return; }
	if (currentFadeIn)
	{
		if (currentFadeVal < endFadeVol)
		{
			currentFadeVal += static_cast<int>(currentFadeOff);
			currentFadePlayer->SetPlayerVolume(currentFadeVal);
		}
		else
		{
			currentFadeVal = endFadeVol;
			currentFadePlayer->SetPlayerVolume(endFadeVol);
			KillTimer(hwnd, 26);
		}
	}
	else
	{
		if (currentFadeVal > endFadeVol)
		{
			currentFadeVal -= static_cast<int>(currentFadeOff);
			currentFadePlayer->SetPlayerVolume(currentFadeVal);
		}
		else
		{
			currentFadeVal = endFadeVol;
			currentFadePlayer->SetPlayerVolume(endFadeVol);
			KillTimer(hwnd, 26);
		}
	}
}

void IPlayer::DrawFFTOnHDCSmall(HDC hdc)
{
	m_outputer->DrawFFTOnHDCSmall(hdc);
}

void IPlayer::SetFFTHDC(HDC hdc)
{
	ffthdc = hdc;
	m_outputer->SetFFTHDC(hdc);
}
void IPlayer::DrawFFTOnHDC(HDC hdc)
{
	m_outputer->DrawFFTOnHDC(hdc);
}

bool IPlayer::err(wchar_t const* errmsg)
{
	wcscpy_s(lasterr, errmsg);
	return false;
}


