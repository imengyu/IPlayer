#pragma once
#include "CSoundOutPuter.h"
#include "CSoundFFT.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <Windows.h>
#include <MMSystem.h>

#include <dsound.h>  

#define WAVE_FORMAT_PCM 1
#define MAX_AUDIO_BUF 4   
#define BUFFERNOTIFYSIZE 192000   
#define ROW_LOCAL_COUNT 32

#define DELAY 1
#define XINTERVAL 1
#define YINTERVAL 1
#define XINTERVALSMALL 1
#define YINTERVALSMALL 1

typedef HRESULT (WINAPI *DirectSoundCreate8Fun)(__in_opt LPCGUID pcGuidDevice, __deref_out LPDIRECTSOUND8 *ppDS8, __null LPUNKNOWN pUnkOuter);

class CDSOutputer :
	public CSoundOutPuter
{
public:
	CDSOutputer(CSoundPlayer * instance);
	~CDSOutputer();

	bool OnCopyData(CSoundPlayer *instance, LPVOID buf, DWORD  buf_len) override;

	bool Create(HWND hWnd, ULONG sample_rate = 44100,  //PCM sample rate  
		int channels = 2,       //PCM channel number  
		int bits_per_sample = 16) override;
	bool Destroy() override;

	HDC ffthdc;



	ULONG sample_rate = 44100;  //PCM sample rate  
	int channels = 2;         //PCM channel number  
	int bits_per_sample = 16; //bits per sample  
	bool drawFFT = false;

	DWORD GetCurrPosSample()override;
	double GetCurrPos()override;
	DWORD GetOutPutingPosSample()override;
	double GetOutPutingPos()override;
	void SetFFTHDC(HDC hdc)override;
	void DrawFFTOnHDC(HDC hdc)override;
	void DrawFFTOnHDCSmall(HDC hdc)override;
	bool IsOutPuting() override;
	bool StartOutPut() override;
	bool EndOutPut() override;
	bool ResetOutPut() override;

	int GetVol() override;
	void SetVol(int vol) override;

	DWORD cur_decorder_pos;
private:
	bool last_data = false;
	bool next_data_end = false;
	bool closed = false;

	DWORD bfs, bfs2;
	CSoundFFT * m_FFT;
	CSoundPlayer * parent;
	IDirectSoundBuffer8 *m_pDSBuffer8 = NULL; //used to manage sound buffers.  
	IDirectSoundBuffer *m_pDSBuffer = NULL;
	IDirectSoundNotify8 *m_pDSNotify = NULL;
	DSBPOSITIONNOTIFY m_pDSPosNotify[MAX_AUDIO_BUF];
	HANDLE m_event[MAX_AUDIO_BUF + 2];

	DSBUFFERDESC dsbd;
	HANDLE playThread;
	DWORD hThreadID;

	void CopyFFT(LPVOID buf, DWORD  buf_len);
	static int WINAPI ThreadFuncPCM(void* lpdwParam);
	DWORD offset = 0;
	DWORD res = WAIT_OBJECT_0;
	HANDLE hThreadEventReset;
	HANDLE hThreadEventExit;
	HANDLE hThreadEventEndOutPut;
	int m_setedVol = 0;
	bool m_outputing = false;

	bool Signaled = true;
	const int LineViewWidth = 200, LineViewHeight = 100;
	const int LineViewSmallWidth = 100, LineViewSmallHeight = 64;
	int local[ROW_LOCAL_COUNT];
	int top_local[ROW_LOCAL_COUNT];
	int top_local_count[ROW_LOCAL_COUNT];
	double row_local_table[ROW_LOCAL_COUNT] = {
		0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4,2.6,2.8,
		3.0,3.2,3.5,4.0,4.5,5,6,7,8,9,10,15,20,25,30,35,40
	};
	double row_local_table_small[ROW_LOCAL_COUNT] = {
		0.0,0.5,1.0,1.5,2,2.5,3,3.4,3.6,3.8,4.0,4.2,4.6,
		4.8,5,5.2,5.6,6.0,6.3,6.7,8,9,10,15,20,25,30,40
	};

	BYTE fft_datas[MAX_AUDIO_BUF*BUFFERNOTIFYSIZE];
	HBRUSH hBrushGray;
	HBRUSH hBrushBlack;
	HBRUSH hBrushGreen;
	HBRUSH hBrushRed;
};

