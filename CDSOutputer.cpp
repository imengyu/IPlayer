#include "stdafx.h"
#include "CDSOutputer.h"
#include "CSoundPlayer.h"
#include "IPlayer.h"
#include <math.h>
#include "FastFourierTransform.h"
#include "DbgHlper.h"

IDirectSound8 *m_pDS = NULL;
bool dsLoaded = false;
DirectSoundCreate8Fun f_ds;
HMODULE hDs;
WCHAR testdatastr[64];

//pre=10^(db/2000)*100
//log10(x)=y;
//db=2000.0 * log10(pre/100);
//pre=db/2000

#define PRE_TO_DB(pre) 2000.0 * log10(pre/100.0)
#define DB_TO_PRE(db) pow(10, db / 2000.0)*100.0

CDSOutputer::CDSOutputer(CSoundPlayer * instance)
{
	parent = instance;
	hThreadEventReset = CreateEvent(NULL, TRUE, FALSE, L"ThreadEventReset");
	hThreadEventExit = CreateEvent(NULL, TRUE, TRUE, L"ThreadEventExit");
	hThreadEventEndOutPut = CreateEvent(NULL, TRUE, FALSE, L"ThreadEventEndOutPut");
	m_event[4] = hThreadEventReset;
	m_event[5] = hThreadEventEndOutPut;

	m_FFT = new CSoundFFT();
	hBrushGray = CreateSolidBrush(RGB(255, 50, 50));
	hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
	hBrushGreen = CreateSolidBrush(RGB(0, 1, 0));
	hBrushRed = CreateSolidBrush(RGB(1, 0, 0));
}
CDSOutputer::~CDSOutputer()
{
	Destroy();
	if (playThread)TerminateThread(playThread, 0);
	playThread = NULL;
	CloseHandle(hThreadEventReset);
	CloseHandle(hThreadEventExit);
	CloseHandle(hThreadEventEndOutPut);
	DeleteObject(hBrushGray);
	DeleteObject(hBrushRed);
	DeleteObject(hBrushGreen);
	DeleteObject(hBrushBlack);

	if (m_FFT != NULL) {
		delete[] m_FFT;
		m_FFT = NULL;
	}
}

bool CDSOutputer::OnCopyData(CSoundPlayer * instance, LPVOID buf, DWORD buf_len)
{
	if (callback)return callback(parent,buf, buf_len);
	return false;
}

bool CDSOutputer::Create(HWND hWnd, ULONG sample_rate, int channels,  int bits_per_sample)
{
	closed = false;
	last_data = false;
	next_data_end = false;
	if(hWnd==NULL) return parent->err(L"Init DirectSound failed : hWnd can not be nullptr!");
	this->sample_rate = sample_rate;
	this->channels = channels;
	this->bits_per_sample = bits_per_sample;
	bfs = sample_rate* static_cast<ULONG>(channels) * static_cast<ULONG>(bits_per_sample)/8;
	bfs2 = static_cast<ULONG>(channels) * static_cast<ULONG>(bits_per_sample) / 8;
	if (!dsLoaded) {
		hDs = LoadLibrary(L"dsound.dll");
		if (!hDs)
			return parent->err(L"Init DirectSound failed : Load dsound.dll failed!");
		f_ds = (DirectSoundCreate8Fun)GetProcAddress(hDs, "DirectSoundCreate8");
		if (FAILED(f_ds(NULL, &m_pDS, NULL)))
			return parent->err(L"Init DirectSound failed : DirectSoundCreate8 failed!");
		if (FAILED(m_pDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
			return parent->err(L"Init DirectSound failed : SetCooperativeLevel");
		dsLoaded = true;
	}

	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2| DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = MAX_AUDIO_BUF * BUFFERNOTIFYSIZE;
	//WAVE Header  
	dsbd.lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
	dsbd.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	/* format type */
	(dsbd.lpwfxFormat)->nChannels = static_cast<WORD>(channels);
	/* number of channels (i.e. mono, stereo...) */
	(dsbd.lpwfxFormat)->nSamplesPerSec = sample_rate;
	/* sample rate */
	(dsbd.lpwfxFormat)->nAvgBytesPerSec = sample_rate * static_cast<DWORD>((bits_per_sample / 8)*channels);
	/* for buffer estimation */
	(dsbd.lpwfxFormat)->nBlockAlign = static_cast<WORD>((bits_per_sample / 8)*channels);
	/* block size of data */
	(dsbd.lpwfxFormat)->wBitsPerSample = static_cast<WORD>(bits_per_sample);
	/* number of bits per sample of mono data */
	(dsbd.lpwfxFormat)->cbSize = 0;

	//Creates a sound buffer object to manage audio samples.   
	if (FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL))) {
		return parent->err(L"DirectSound error : CreateSoundBuffer failed!");
	}

	if (FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8))) {
		return parent->err(L"DirectSound error.");
	}
	else if (m_setedVol) {
		SetVol(m_setedVol);
		m_setedVol = 0;
	}
	//Get IDirectSoundNotify8  
	if (FAILED(m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))) {
		return parent->err(L"DirectSound error.");
	}
	for (int i = 0; i<MAX_AUDIO_BUF; i++) {
		m_pDSPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
		m_event[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pDSPosNotify[i].hEventNotify = m_event[i];
	}
	m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);
	m_pDSNotify->Release();

	offset = 0;
	cur_decorder_pos = 0;

	OutputDebugString(L"Created\n");

	return true;
}
bool CDSOutputer::Destroy()
{
	last_data = false;
	next_data_end = false;
	offset = 0;
	cur_decorder_pos = 0;
	EndOutPut();

	free(dsbd.lpwfxFormat);
	if (m_pDSBuffer8) {
		m_pDSBuffer8->Stop();
		m_pDSBuffer8->Release();
	}
	m_pDSBuffer8 = NULL;
	if (m_pDSBuffer) {
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
	}
	m_pDSBuffer = NULL;
	closed = true;
	OutputDebugString(L"Destroyed\n");
	return false;
}

DWORD CDSOutputer::GetCurrPosSample()
{
	if (m_outputing)
		return cur_decorder_pos + GetOutPutingPosSample();
	return 0;
}
double CDSOutputer::GetCurrPos()
{
	if (m_outputing)
		return ((double)cur_decorder_pos / (double)sample_rate) + GetOutPutingPos();
	return 0.0;
}
DWORD CDSOutputer::GetOutPutingPosSample()
{
	if (m_outputing)
	{
		DWORD cp;
		if (SUCCEEDED(m_pDSBuffer8->GetCurrentPosition(&cp, NULL))) {
			return cp / bfs2;
		}
	}
	return 0;
}
double CDSOutputer::GetOutPutingPos()
{
	if (m_outputing)
	{
		DWORD cp;
		if (SUCCEEDED(m_pDSBuffer8->GetCurrentPosition(&cp, NULL))) {
			return  (double)cp / (double)bfs;
		}
	}
	return 0.0;
}

void CDSOutputer::SetFFTHDC(HDC hdc)
{
	if (hdc)
		drawFFT = true;
	else 
		drawFFT = false;
	ffthdc = hdc;
}
void CDSOutputer::DrawFFTOnHDC(HDC hdc)
{
	if (m_FFT != NULL && m_pDSBuffer8 != NULL)
	{
		HGDIOBJ old = SelectObject(hdc, hBrushBlack);
		DWORD pl, wr;
		if (SUCCEEDED(m_pDSBuffer8->GetCurrentPosition(&pl, &wr)))
		{
			pl %= BUFFERNOTIFYSIZE;
			if (pl + FFT_SIZE < sizeof(fft_datas))
				m_FFT->spectrogram(fft_datas, sample_rate, pl);
			else m_FFT->spectrogram(fft_datas, sample_rate, sizeof(fft_datas) - FFT_SIZE);

			int db = m_FFT->db(fft_datas, pl, FFT_SIZE);

			//wchar_t s[10];
			//wsprintf(s, L"%d\n",db); OutputDebugString(s);

			double *real = m_FFT->real;
			double *imag = m_FFT->imag;

			double model;
			//计算绘制频谱格子的宽度  
			float x_step = ((float)LineViewWidth / SPECTROGRAM_COUNT);
			//格子的高度  
			float y_step = (float)LineViewHeight / ROW_LOCAL_COUNT;

			//SPECTROGRAM_COUNT = 31（取采样频率点数）  
			for (int i = 0; i < SPECTROGRAM_COUNT; i++) {
				model = 2 * hypot(real[i], imag[i]) / FFT_SIZE;// 计算电频最大值，Math.hypot(x,y)返回sqrt(x2+y2)，最高电频  
				for (int k = 1; k < ROW_LOCAL_COUNT; k++) {
					if (model >= row_local_table[k - 1]
						&& model < row_local_table[k]) {
						local[i] = k - 1;//这里取最高电频所对应的方格数  
						break;
					}
				}				
				if (db > 40)
					local[i] = (int)((float)local[i] * (float)(db - 40) / 40.0F);
				else local[i] = (int)((float)local[i] * 0.1F);
				//ROW_LOCAL_COUNT = 32（最多可绘制的方格数），这里采用的是逆向思维绘制，y = 0对应最大方格数32  
				local[i] = ROW_LOCAL_COUNT - local[i];

				// 柱形  
				if (Signaled) {
					//mPaint.setColor(sepColor);
					//mPaint.setAlpha(getSepAlpha());
				}
				else {

				}
				float x = XINTERVAL + i * x_step;
				//从下往上绘制方格  
				for (int j = ROW_LOCAL_COUNT; j > local[i]; j--) {
					float y = (j - 1) * y_step;
					Rectangle(hdc, (int)x, (int)y, (int)(x + x_step - XINTERVAL), (int)(y + y_step - YINTERVAL));// 绘制矩形  
				}
				// 绿点  
				if (Signaled) {

				}
				else {

				}
				//下面部分是用来显示落差效果的，没有强大的理解能力可能会绕晕，所以我也不做过多注释，看个人天赋吧  
				//local[i] < top_local[i]说明最高点改变（local[i]越小，点越高，这里需要注意）  
				if (local[i] < top_local[i]) {
					//当进入到这里，则说明需要更新最大电频，这个可能发生在上一次最大电频方格下降的过程中  
					top_local[i] = local[i];
					top_local_count[i] = 0;//清零，如果top值未更新，则循环10次开始下落  
				}
				else {
					top_local_count[i]++;
					//这里top_local_count这个是用来记录达到top值的柱体，然后会循环10次才开始下落  
					// top_local_count中小于DELAY的top_local都保持不变  
					if (top_local_count[i] >= DELAY) {
						top_local_count[i] = DELAY;
						//这里控制下降的速度  
						top_local[i] = local[i] > (top_local[i] + 1) ? (top_local[i] + 1) : local[i];
					}
				}
				//y增加则最高位方格下降  
				float y = top_local[i] * y_step;			
				SelectObject(hdc, hBrushGray);
				Rectangle(hdc, (int)x, (int)y, (int)(x + x_step - XINTERVAL), (int)(y + y_step - YINTERVAL));// 最高位置的方格  
				SelectObject(hdc, hBrushBlack);
			}
		}
		SelectObject(hdc, old);
	}
}
void CDSOutputer::DrawFFTOnHDCSmall(HDC hdc)
{
	if (m_FFT != NULL && m_pDSBuffer8 != NULL)
	{
		HGDIOBJ old = SelectObject(hdc, hBrushBlack);
		DWORD pl, wr;
		if (SUCCEEDED(m_pDSBuffer8->GetCurrentPosition(&pl, &wr)))
		{
			pl %= BUFFERNOTIFYSIZE;
			if (pl + FFT_SIZE < sizeof(fft_datas))
				m_FFT->spectrogram_nostanderd(fft_datas, sample_rate, pl);
			else m_FFT->spectrogram_nostanderd(fft_datas, sample_rate, sizeof(fft_datas) - FFT_SIZE);

			int db = m_FFT->db(fft_datas, pl, FFT_SIZE);

			double *real = m_FFT->real;
			double *imag = m_FFT->imag;

			double model;
			//计算绘制频谱格子的宽度  
			float x_step = ((float)LineViewSmallWidth / SPECTROGRAM_COUNT_SMALL);
			float y_step = 1;

			//SPECTROGRAM_COUNT = 31（取采样频率点数）  
			for (int i = 0; i < SPECTROGRAM_COUNT; i++) {
				model = 2 * hypot(real[i], imag[i]) / FFT_SIZE;// 计算电频最大值，Math.hypot(x,y)返回sqrt(x2+y2)，最高电频  
				local[i] = 0;
				for (int k = 1; k < ROW_LOCAL_COUNT; k++) {
					if (model >= row_local_table_small[k - 1]
						&& model < row_local_table_small[k]) {
						local[i] = k - 1;//这里取最高电频所对应的方格数  
						break;
					}
				}
				if (db > 60)
					local[i] = (int)((float)local[i] * ((float)(db - 60) / 20.0F));
				else local[i] = (int)((float)local[i] * 0.1F);
				local[i] = ROW_LOCAL_COUNT - local[i];
				float x = XINTERVALSMALL + i * x_step;
				Rectangle(hdc, (int)x, LineViewSmallHeight, (int)(x + x_step - XINTERVALSMALL), (int)((local[i] - 1) * y_step + y_step - YINTERVALSMALL));
				//下面部分是用来显示落差效果的，没有强大的理解能力可能会绕晕，所以我也不做过多注释，看个人天赋吧  
				//local[i] < top_local[i]说明最高点改变（local[i]越小，点越高，这里需要注意）  
				if (local[i] < top_local[i]) {
					//当进入到这里，则说明需要更新最大电频，这个可能发生在上一次最大电频方格下降的过程中  
					top_local[i] = local[i];
					top_local_count[i] = 0;//清零，如果top值未更新，则循环10次开始下落  
				}
				else {
					top_local_count[i]++;
					//这里top_local_count这个是用来记录达到top值的柱体，然后会循环10次才开始下落  
					// top_local_count中小于DELAY的top_local都保持不变  
					if (top_local_count[i] >= DELAY) {
						top_local_count[i] = DELAY;
						//这里控制下降的速度  
						top_local[i] = local[i] > (top_local[i] + 1) ? (top_local[i] + 1) : local[i];
					}
				}
				//y增加则最高位方格下降  
				float y = top_local[i] * y_step;
				SelectObject(hdc, hBrushGray);
				Rectangle(hdc, (int)x, (int)y, (int)(x + x_step), (int)(y + y_step));// 最高位置的方格
				SelectObject(hdc, hBrushBlack);
			}
		}
		SelectObject(hdc, old);
	}
}
bool CDSOutputer::IsOutPuting()
{
	return m_outputing;
}
bool CDSOutputer::StartOutPut()
{	
	if (!m_outputing)
	{
		if (playThread)
			WaitForSingleObject(m_event[6], INFINITE);
		ResetEvent(m_event[5]);
		ResetEvent(m_event[6]);
		m_outputing = true;
		playThread = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFuncPCM, (void*)this, 0, &hThreadID);
		if (playThread == NULL)
			return parent->err(L"Failed create player thread.");
		cur_decorder_pos = 0;
		m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);
		m_pDSBuffer8->SetCurrentPosition(0);
		return m_outputing;
	}
	return m_outputing;
}
bool CDSOutputer::EndOutPut()
{
	if (m_outputing) {
		SetEvent(m_event[5]);
		m_pDSBuffer8->Stop();
	}
	return true;
}
bool CDSOutputer::ResetOutPut()
{
	if (m_outputing)
	{
		//cur_decorder_pos = ((IPlayer*)parent)->UpdatePos();
		m_pDSBuffer->Stop();
		SetEvent(m_event[4]);
		return true;
	}
	else return StartOutPut();
}

int CDSOutputer::GetVol()
{
	LONG lg = 0;
	if (m_pDSBuffer8 != NULL) {
		m_pDSBuffer8->GetVolume(&lg);
	}
	return static_cast<int>(DB_TO_PRE(lg));
}
void CDSOutputer::SetVol(int vol)
{
	if (m_pDSBuffer8 != NULL) {
		if (vol != 0) m_pDSBuffer8->SetVolume((LONG)(PRE_TO_DB(vol)));
		else m_pDSBuffer8->SetVolume((LONG)(-10000));
	}
	else {
		m_setedVol = vol;
	}
}

void CDSOutputer::CopyFFT(LPVOID buf, DWORD buf_len)
{
	//DWORD pl, wr;
	//if (SUCCEEDED(m_pDSBuffer8->GetCurrentPosition(&pl, &wr)))
	//{
		memcpy_s(fft_datas, sizeof(fft_datas), buf, buf_len);
	//}
}

int CDSOutputer::ThreadFuncPCM(void * lpdwParam)
{
	LPVOID buf = NULL;
	DWORD bytes = 0;

	CDSOutputer*instance = static_cast<CDSOutputer*>(lpdwParam);
	OutputDebugString(L"ThreadStart\n");
	if (instance) {
		RESET:
		instance->offset = 0;
		instance->res = 0;

		while (instance->m_outputing) 
		{
			switch (instance->res)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				if (SUCCEEDED(instance->m_pDSBuffer8->Lock(instance->offset, BUFFERNOTIFYSIZE, &buf, &bytes, NULL, NULL, NULL)))
				{
					instance->m_outputing = instance->OnCopyData(instance->parent, buf, bytes);
					instance->m_pDSBuffer8->Unlock(buf, bytes, NULL, NULL);
					instance->offset += BUFFERNOTIFYSIZE;
					instance->offset %= BUFFERNOTIFYSIZE * MAX_AUDIO_BUF;
				}
			    break;
			case 4:
				ResetEvent(instance->m_event[4]);
				instance->m_pDSBuffer8->Stop();
				instance->m_pDSBuffer8->SetCurrentPosition(0);
				instance->m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);
				goto RESET;
				break;
			case 5:				
				ResetEvent(instance->m_event[5]);
				goto EXIT;
				break;
			default:
				break;
			}

			instance->res = WaitForMultipleObjects(MAX_AUDIO_BUF + 2, instance->m_event, FALSE, INFINITE);
		}
	}
EXIT:
	instance->m_outputing = false;
	SetEvent(instance->m_event[5]);
	SetEvent(instance->m_event[6]);
	OutputDebugString(L"ExitThread\n");
	ExitThread(0);
	return 0;
}


