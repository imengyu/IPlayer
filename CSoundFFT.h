#pragma once
#include "stdafx.h"

#define PI_2 6.283185F
#define PI   3.1415925F
#define FFT_SIZE 512
#define SPECTROGRAM_COUNT 31
#define SPECTROGRAM_COUNT_SMALL 21

class CSoundFFT
{
public:
	CSoundFFT();
	~CSoundFFT();

	/*
	* 获取所有振幅之平均值 计算db (振幅最大值 2^16-1 = 65535 最大值是 96.32db) 
	*
	* @param pcmdata
	* @param size 大小
	*/
	int db(const unsigned char * pcmdata, size_t startpos, size_t size);
	int getPcmDB(const unsigned char * pcmdata, size_t size);
	/**
	* 显示频谱时进行FFT计算
	*
	* @param buf
	* @param samplerate 采样率
	*/	
	void spectrogram(BYTE buf[], double samplerate, int startpos = 0);

	void spectrogram_nostanderd(BYTE buf[], double samplerate, int startpos);

	/**
	* 快速傅立叶变换，将复数 x 变换后仍保存在 x 中(这个算法可以不用理解，直接用)，转成频率轴的数（呈线性分步）
	* 计算出每一个点的信号强度，即电频强度
	*
	* @param real 实部
	* @param imag 虚部
	* @param n    多少个数进行FFT,n必须为2的指数倍数
	* @return
	*/
	int fft(double real[], double imag[], int n);

	bool locinited = false;

	int loc[SPECTROGRAM_COUNT];
	double sampleratePoint[SPECTROGRAM_COUNT];
	double real[FFT_SIZE];
	double imag[FFT_SIZE];

	double first_fft_real[FFT_SIZE];
	double first_fft_imag[FFT_SIZE];

	double second_fft_real[FFT_SIZE];
	double second_fft_imag[FFT_SIZE];
};

