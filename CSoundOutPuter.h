#pragma once
#include "stdafx.h"

class CSoundPlayer;

typedef bool(*OnCopyDataFun)(CSoundPlayer *instance, LPVOID buf, DWORD  buf_len);

class CSoundOutPuter
{
public:
	CSoundOutPuter();
	~CSoundOutPuter();

	OnCopyDataFun callback;

	void SetOnCopyDataCallback(OnCopyDataFun callback);

	virtual bool OnCopyData(CSoundPlayer *instance, LPVOID buf, DWORD  buf_len);

	virtual bool Create(HWND hWnd, ULONG sample_rate = 44100,  //PCM sample rate  
		int channels = 2,       //PCM channel number  
		int bits_per_sample = 16);
	virtual bool Destroy();

	virtual double GetOutPutingPos() { return 0; };
	virtual DWORD GetOutPutingPosSample() { return 0; };
	virtual DWORD GetCurrPosSample() { return 0; };
	virtual  double GetCurrPos() { return 0; };

	virtual int GetVol();
	virtual void SetVol(int vol);

	virtual void SetFFTHDC(HDC hdc) {}
	virtual void DrawFFTOnHDC(HDC hdc) {}
	virtual void DrawFFTOnHDCSmall(HDC hdc) {}
	virtual bool IsOutPuting();
	virtual bool StartOutPut();
	virtual bool EndOutPut();
	virtual bool ResetOutPut();
};

