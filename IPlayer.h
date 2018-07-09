#pragma once
#include "CSoundPlayer.h"
#include "CSoundDecoder.h"
#include "CSoundOutPuter.h"
#include <list>

#define WAVE_IN_NUM 15

class CDecoderPluginReal : public CDecoderPlugin
{
public:
	_CreateFun CreateFun;
	_DestroyFun DestroyFun;
	HMODULE hModule = NULL;

	bool operator==(const CDecoderPluginReal right);
	bool operator!=(const CDecoderPluginReal right);
	bool operator==(const TStreamFormat right);
	bool operator!=(const TStreamFormat right);
	bool operator==(const LPWSTR right);
	bool operator!=(const LPWSTR right);
};

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

	CSoundDecoder*GetCurrDecoder();
	TStreamFormat GetFormat();
	LPWSTR GetMusicPosTimeString();
	LPWSTR GetMusicLenTimeString();
	LPWSTR GetMusicTimeString();
	TPlayerStatus GetPlayerStatus();

	DWORD GetMusicPosSample();
	DWORD SetMusicPosSample(DWORD sample);

	double GetMusicPos();
	double GetMusicLength();
	DWORD GetMusicLengthSample();
	bool SetMusicPos(double sec);

	void FadeOut(int sec, int from, int to);
	void FadeIn(int sec, int from, int to);

	void SetDefOutPutSettings(ULONG sample_rate, int channels, int bits_per_sample);

	int GetPlayerVolume();
	void SetPlayerVolume(int vol);
	bool IsPlayingMidi();
	bool LoadDecoderPlugin(TStreamFormat format, LPWSTR path, LPCSTR createFun, LPCSTR destroyFun);
	bool UnLoadDecoderPlugin(TStreamFormat format, LPWSTR path);
	CSoundDecoder* CreateDecoderInPlugin(TStreamFormat format);
	bool DestroyDecoderInPlugin(TStreamFormat format, CSoundDecoder * d);
	TStreamFormat GetFileFormat(const wchar_t *pchFileName);
	LPWSTR GetLastErr() {
		return lasterr;
	}

	HWND hostHWnd;

	DWORD GetMusicSampleRate() override { if(m_decoder) return m_decoder->GetMusicSampleRate(); return 0;}
	int GetMusicBitPerSample() override { if (m_decoder) return m_decoder->GetMusicBitsPerSample(); return 0; }
	int GetMusicChannelsCount() override { if (m_decoder) return m_decoder->GetMusicChannelsCount(); return 0; }

	void DrawFFTOnHDCSmall(HDC hdc);
	void SetFFTHDC(HDC hdc)override;
	void DrawFFTOnHDC(HDC hdc)override;

	ULONG get_def_sample_rate() { return def_sample_rate; }
	int get_def_channels() { return def_channels; }
	int get_def_bits_per_sample() { return def_bits_per_sample; }

	bool err(wchar_t const* errmsg);
	bool defoutseted() { return def_out_seted; }

	void SetEndStatus();
	DWORD UpdatePos();
protected:
	CSoundDecoder *CreateDecoderWithFormat(TStreamFormat f);
private:
	WCHAR lasterr[256];
	WCHAR isplayingPosTime[32];
	WCHAR isplayingTime[32];
	WCHAR isplayingAllTime[16];
	HDC ffthdc;
	BOOL m_playingMidi = FALSE,
		m_LastDestroyed = TRUE,
		m_NextStop = FALSE;

	CSoundOutPuter * m_outputer;
	CSoundDecoder *m_decoder;
	TPlayerStatus m_playerStatus;

	ULONG def_sample_rate;
	int def_channels;
	int def_bits_per_sample;
	bool def_out_seted = false;
	
	int playerVol = 100;
	char *c_user_mixer_line;
	unsigned int c_user_mixer_line_id;
	unsigned int c_user_mixer_volume;

	unsigned int c_nWaveInBufferSize;  // maximal size(latency) of wave in buffer in milliseconds

	TStreamFormat m_openedFileFormat = TStreamFormat::sfUnknown;



	static bool OnCopyData(CSoundPlayer*instance, LPVOID buf, DWORD  buf_len);


	static VOID CALLBACK OnFadeTime(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);

};


