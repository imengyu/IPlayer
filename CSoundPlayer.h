#pragma once
#include "stdafx.h"
#include "CSoundDecoder.h"

//音频格式（目前只支持几种）
enum TStreamFormat
{
	sfUnknown = 0,
	sfMp3 = 1,//已支持MP3
	sfOgg = 2,//已支持OGG
	sfWav = 3,//已支持WAV
	sfPCM = 4,//已支持PCM
	sfFLAC = 5,//已支持FLAC
	sfFLACOgg = 6,//已支持FLAC  Vobis
	sfAC3 = 7,
	sfAacADTS = 8,
	sfWaveIn = 9,//not used
	sfMidi = 10,//已支持MIDI
	sfWma = 11,//已支持wma
	sfM4a = 12,
	sfAutodetect = 1000//not used
};

//播放器状态
enum TPlayerStatus
{
	Unknow,
	//未打开文件
	NotOpen,
	//文件已打开就绪
	Opened,
	//正在播放
	Playing,
	//已暂停
	Paused,
	//播放到音乐结尾了
	PlayEnd,
};

class CDecoderPlugin
{
public:
	TStreamFormat TargetFormat = TStreamFormat::sfUnknown;
	LPWSTR DllPath;
	CSoundDecoder*Decoder = NULL;

};

typedef CSoundDecoder*(__cdecl*_CreateFun)(CDecoderPlugin*pu);
typedef BOOL(__cdecl*_DestroyFun)(CDecoderPlugin*pu, CSoundDecoder*de);

//播放器
class CSoundPlayer
{
public:
	CSoundPlayer();
	~CSoundPlayer();

	//加载解码器插件
	bool LoadDecoderPlugin(TStreamFormat format, LPWSTR path, LPCSTR createFun, LPCSTR destroyFun) { return false; };
	//卸载解码器插件
	bool UnLoadDecoderPlugin(TStreamFormat format, LPWSTR path) { return false; };

	//打开音乐
	// *file ：音乐路径
	virtual bool Open(LPWSTR file) { return false; }
	//关闭音乐
	virtual bool Close() { return false; }
	/*
	播放或暂停音乐
	* 如果音乐正在播放，则暂停音乐；如果音乐暂停，则开始播放音乐（相当于播放/暂停按钮）。
	*/
	virtual bool PausePlay() { return false; }
	//开始播放
	virtual bool Play() { return false; }
	//暂停播放
	virtual bool Pause() { return false; }
	//停止播放
	virtual bool Stop() { return false; }
	//从头开始播放
	virtual bool Restart() { return false; }

	//获取当前解码器
	virtual CSoundDecoder*GetCurrDecoder();

	//音乐淡出
	//* sec ：淡出需要的时间
	//* from ：当前音量
	//* to：结尾音量
	//* 相当于音量慢慢变小
	virtual void FadeOut(int sec, int from, int to) {}
	/*音乐淡入
	* sec ：淡入需要的时间
	* from ：当前音量
	* to：结尾音量
	* 相当于音量慢慢变大
	*/
	virtual void FadeIn(int sec, int from, int to) {}

	//获取当前已打开音乐的格式
	virtual TStreamFormat GetFormat() { return (TStreamFormat)0; }
	//获取播放器状态
	virtual TPlayerStatus GetPlayerStatus() { return (TPlayerStatus)0; }

	//获取播放器上一个错误信息
	virtual LPWSTR GetLastErr();

	//获取当前音乐的长度
	// * 以 sample 为单位
	virtual DWORD GetMusicLengthSample() { return 0; }
	//获取音乐播放的位置
	// * 以 秒 为单位
	virtual double GetMusicPos() { return 0; }
	//获取当前音乐的长度
	// * 以 秒 为单位
	virtual double GetMusicLength() { return 0; }
	/*获取音乐正在播放的时间字符串
	* 格式是：00:00/00:00 正在播放时间/音乐时长
	*/
	virtual LPWSTR GetMusicTimeString() { return 0; }
	/*获取音乐正在播放的时间字符串
	* 格式是：00:00 正在播放时间
	*/
	virtual LPWSTR GetMusicPosTimeString() { return 0; }
	/*获取音乐时长字符串
	* 格式是：00:00 音乐时长
	*/
	virtual LPWSTR GetMusicLenTimeString() { return 0; }

	//获取音乐播放的样本位置
	// * 以 样本 为单位
	// * 使用 GetMusicSampleRate 获取 音乐采样率
	virtual DWORD GetMusicPosSample() { return 0; }
	//设置音乐播放的位置
	// * 以 样本 sample 为单位
	virtual DWORD SetMusicPosSample(DWORD sample) { return 0; }

	//获取音乐文件的 采样率
	virtual DWORD GetMusicSampleRate() { return 0; }
	//获取音乐文件的 量化位数 （8/16/32）
	virtual int GetMusicBitPerSample() { return 0; }
	//获取音乐文件的 声道数
	virtual int GetMusicChannelsCount() { return 0; }

	virtual void SetFFTHDC(HDC hdc) {}
	virtual void DrawFFTOnHDC(HDC hdc) {}
	virtual void DrawFFTOnHDCSmall(HDC hdc) {}
	//设置音乐播放的位置
	// * 以 秒 为单位
	virtual bool SetMusicPos(double sec) { return false; }
	//获取音乐是否已打开
	virtual bool IsOpened() { return false; }
	/*获取播放器音量
	* 返回音量数值 ：1-100
	*/
	virtual int GetPlayerVolume() { return 0; }
	/*设置播放器音量
	* vol：音量数值 （1-100）
	*/
	virtual void SetPlayerVolume(int vol) {  }
	/*设置默认音频输出参数
	* sample_rate 采样率
	* channels 声道数
	* bits_per_sample 采样位数
	*/
	virtual void SetDefOutPutSettings(ULONG sample_rate, int channels, int bits_per_sample) {};

	//获取是否正在播放midi音乐
	virtual bool IsPlayingMidi() { return false; };
	//设置错误信息
	// * errmsg：错误信息
	virtual bool err(LPWSTR errmsg) { return false; }
};

