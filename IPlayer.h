#pragma once
#include "CSoundPlayer.h"
#include "CSoundDecoder.h"
#include "CSoundOutPuter.h"

#define WAVE_IN_NUM 15

class IPlayer : public CSoundPlayer
{
public:
	IPlayer(HWND hWnd);
	~IPlayer();

	bool Open(LPWSTR file);
	bool Close();
	bool PausePlay();
	bool Play();
	bool Pause();
	bool Stop();
	bool Restart();
	bool IsOpened();
	
	TStreamFormat GetFormat();
	LPWSTR GetMusicTimeString();
	TPlayerStatus GetPlayerStatus();

	double GetMusicPos();
	double GetMusicLength();
	bool SetMusicPos(double sec);

	void FadeOut(int sec, int from, int to);
	void FadeIn(int sec, int from, int to);

	int GetPlayerVolume();
	void SetPlayerVolume(int vol);
	bool IsPlayingMidi();
	TStreamFormat GetFileFormat(const wchar_t *pchFileName);
	LPWSTR GetLastErr() {
		return lasterr;
	}

	HWND hostHWnd;

	void DrawFFTOnHDCSmall(HDC hdc);
	void SetFFTHDC(HDC hdc)override;
	void DrawFFTOnHDC(HDC hdc)override;

	bool err(wchar_t const* errmsg);
protected:
	CSoundDecoder *CreateDecoderWithFormat(TStreamFormat f);
private:
	WCHAR lasterr[256];
	WCHAR isplayingTime[32];
	WCHAR isplayingAllTime[16];
	HDC ffthdc;
	BOOL playingMidi = FALSE;

	CSoundOutPuter * m_outputer;
	CSoundDecoder *m_decoder;
	TPlayerStatus m_playerStatus;

	int playerVol = 100;
	char *c_user_mixer_line;
	unsigned int c_user_mixer_line_id;
	unsigned int c_user_mixer_volume;

	unsigned int c_nWaveInBufferSize;  // maximal size(latency) of wave in buffer in milliseconds

	TStreamFormat m_openedFileFormat = TStreamFormat::sfUnknown;



	static bool OnCopyData(CSoundPlayer*instance, LPVOID buf, DWORD  buf_len);


	static VOID CALLBACK OnFadeTime(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);

};


