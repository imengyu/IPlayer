#include "stdafx.h"
#include "CSoundFFT.h"
#include <math.h>


CSoundFFT::CSoundFFT()
{
	memset(&first_fft_real, 0, sizeof(first_fft_real));
	memset(&first_fft_imag, 0, sizeof(first_fft_imag));
	memset(&second_fft_real, 0, sizeof(second_fft_real));
	memset(&second_fft_imag, 0, sizeof(second_fft_imag));

	memset(&real, 0, sizeof(real));
	memset(&imag, 0, sizeof(imag));

}
CSoundFFT::~CSoundFFT()
{

}

int CSoundFFT::db(const unsigned char *pcmdata, size_t startpos, size_t size) {

	int db = 0;
	short int value = 0;
	double sum = 0;
	for (UINT i = startpos; i < startpos + size; i += 2)
	{
		memcpy(&value, pcmdata + i, 2); //获取2个字节的大小（值）  
		sum += abs(value); //绝对值求和  
	}
	sum = sum / (size / 2); //求平均值（2个字节表示一个振幅，所以振幅个数为：size/2个）  
	if (sum > 0)
	{
		db = (int)(20.0*log10(sum));
	}
	return db;
}
int CSoundFFT::getPcmDB(const unsigned char *pcmdata, size_t size) {

	int db = 0;
	short int value = 0;
	double sum = 0;
	for (UINT i = 0; i < size; i += 2)
	{
		memcpy(&value, pcmdata + i, 2); //获取2个字节的大小（值）  
		sum += abs(value); //绝对值求和  
	}
	sum = sum / (size / 2); //求平均值（2个字节表示一个振幅，所以振幅个数为：size/2个）  
	if (sum > 0)
	{
		db = (int)(20.0*log10(sum));
	}
	return db;
}
void CSoundFFT::spectrogram(BYTE buf[], double samplerate, int startpos) {
	first_fft_real[0] = buf[startpos];
	first_fft_imag[0] = 0;

	second_fft_real[0] = buf[startpos];
	second_fft_imag[0] = 0;

	for (int i = 0; i < FFT_SIZE; i++) {
		first_fft_real[i] = buf[i + startpos];
		first_fft_imag[i] = 0;
		// 八分频(相当于降低了8倍采样率)，这样1024缓存区中的fft频率密度就越大，有利于取低频  
		//second_fft_real[i] = (buf[i * 8 + startpos] + buf[i * 8 + 1] + buf[i * 8 + 2 + startpos]
		//	+ buf[i * 8 + 3 + startpos] + buf[i * 8 + 4 + startpos] + buf[i * 8 + 5 + startpos]
		//	+ buf[i * 8 + 6 + startpos] + buf[i * 8 + 7 + startpos]) / 8.0;
		second_fft_real[i] = buf[i + startpos];
		second_fft_imag[i] = 0;
	}
	// 高频部分从原始数据取  
	fft(first_fft_real, first_fft_imag, FFT_SIZE);

	// 八分频后的1024个数据的FFT,频率间隔为5.512Hz(samplerate / 8)，取低频部分  
	fft(second_fft_real, second_fft_imag, FFT_SIZE);
	//这里算出的是每一个频点的坐标，对应横坐标的值，因为是定值，所以只需要算一次  
	if (!locinited) {
		double F = pow(20000 / 20, 1.0 / SPECTROGRAM_COUNT);//方法中20为低频起点20HZ，31为段数  
		for (int i = 0; i < SPECTROGRAM_COUNT; i++) {
			//20000表示的最大频点20KHZ,这里的20-20K之间坐标的数据成对数关系,这是音频标准  
			sampleratePoint[i] = 20 * pow(F, i);//乘方，30为低频起点   
												//这里的samplerate为采样率(samplerate / (1024 * 8))是8分频后点FFT的点密度  
			loc[i] = (int)(sampleratePoint[i] / (samplerate / (1024.0  *8.0)));//估算出每一个频点的位置  
		}
		locinited = true;
	}
	const int LowFreqDividing = 14;
	//低频部分  
	for (int j = 0; j < LowFreqDividing; j++) {
		int k = loc[j];
		// 低频部分：八分频的数据,取31段，以第14段为分界点，小于为低频部分，大于为高频部分  
		// 这里的14是需要取数后分析确定的，确保低频有足够的数可取  
		real[j] = second_fft_real[k]; //这里的real和imag对应fft运算的实部和虚部  
		imag[j] = second_fft_imag[k];
	}
	// 高频部分，高频部分不需要分频  
	for (int m = LowFreqDividing; m < SPECTROGRAM_COUNT; m++) {
		int k = loc[m];
		real[m] = first_fft_real[k / 8];
		imag[m] = first_fft_imag[k / 8];
	}
}
void CSoundFFT::spectrogram_nostanderd(BYTE buf[], double samplerate, int startpos) {
	first_fft_real[0] = buf[startpos];
	first_fft_imag[0] = 0;

	second_fft_real[0] = buf[startpos];
	second_fft_imag[0] = 0;

	for (int i = 0; i < FFT_SIZE; i++) {
		first_fft_real[i] = buf[i + startpos];
		first_fft_imag[i] = 0;
		// 八分频(相当于降低了8倍采样率)，这样1024缓存区中的fft频率密度就越大，有利于取低频  
		//second_fft_real[i] = (buf[i * 8 + startpos] + buf[i * 8 + 1] + buf[i * 8 + 2 + startpos]
		//	+ buf[i * 8 + 3 + startpos] + buf[i * 8 + 4 + startpos] + buf[i * 8 + 5 + startpos]
		//	+ buf[i * 8 + 6 + startpos] + buf[i * 8 + 7 + startpos]) / 8.0;
		second_fft_real[i] = buf[i + startpos];
		second_fft_imag[i] = 0;
	}
	// 高频部分从原始数据取  
	fft(first_fft_real, first_fft_imag, FFT_SIZE);

	// 八分频后的1024个数据的FFT,频率间隔为5.512Hz(samplerate / 8)，取低频部分  
	fft(second_fft_real, second_fft_imag, FFT_SIZE);
	//这里算出的是每一个频点的坐标，对应横坐标的值，因为是定值，所以只需要算一次  
	if (!locinited) {
		double F = pow(20000 / 20, 1.0 / SPECTROGRAM_COUNT);//方法中20为低频起点20HZ，31为段数  
		for (int i = 0; i < SPECTROGRAM_COUNT; i++) {
			//20000表示的最大频点20KHZ,这里的20-20K之间坐标的数据成对数关系,这是音频标准  
			sampleratePoint[i] = 20 * pow(F, i);//乘方，30为低频起点   
												//这里的samplerate为采样率(samplerate / (1024 * 8))是8分频后点FFT的点密度  
			loc[i] = (int)(sampleratePoint[i] / (samplerate / (1024.0  *8.0)));//估算出每一个频点的位置  
		}
		locinited = true;
	}
	const int LowFreqDividing = 14;
	//低频部分  
	for (int j = 0; j < LowFreqDividing; j++) {
		int k = loc[j];
		// 低频部分：八分频的数据,取31段，以第14段为分界点，小于为低频部分，大于为高频部分  
		// 这里的14是需要取数后分析确定的，确保低频有足够的数可取  
		real[j] = second_fft_real[k]; //这里的real和imag对应fft运算的实部和虚部  
		imag[j] = second_fft_imag[k];
	}
	// 高频部分，高频部分不需要分频  
	for (int m = LowFreqDividing; m < SPECTROGRAM_COUNT; m++) {
		int k = loc[m];
		real[m] = first_fft_real[k / 8];
		imag[m] = first_fft_imag[k / 8];
	}
}
int CSoundFFT::fft(double real[], double imag[], int n)
{
	int i, j, l, k, ip;
	int M = 0;
	int le, le2;
	double sR, sI, tR, tI, uR, uI;

	M = (int)(log(n) / log(2));

	// bit reversal sorting  
	l = n / 2;
	j = l;
	// 控制反转，排序，从低频到高频  
	for (i = 1; i <= n - 2; i++) {
		if (i < j) {
			tR = real[j];
			tI = imag[j];
			real[j] = real[i];
			imag[j] = imag[i];
			real[i] = tR;
			imag[i] = tI;
		}
		k = l;
		while (k <= j) {
			j = j - k;
			k = k / 2;
		}
		j = j + k;
	}
	// For Loops  
	for (l = 1; l <= M; l++) { /* loop for ceil{log2(N)} */
		le = (int)pow(2, l);
		le2 = (int)(le / 2);
		uR = 1;
		uI = 0;
		sR = cos(PI / le2);// cos和sin消耗大量的时间，可以用定值  
		sI = -sin(PI / le2);
		for (j = 1; j <= le2; j++) { // loop for each sub DFT  
									 // jm1 = j - 1;  
			for (i = j - 1; i <= n - 1; i += le) {// loop for each butterfly  
				ip = i + le2;
				tR = real[ip] * uR - imag[ip] * uI;
				tI = real[ip] * uI + imag[ip] * uR;
				real[ip] = real[i] - tR;
				imag[ip] = imag[i] - tI;
				real[i] += tR;
				imag[i] += tI;
			} // Next i  
			tR = uR;
			uR = tR * sR - uI * sI;
			uI = tR * sI + uI * sR;
		} // Next j  
	} // Next l  

	return 0;
}
