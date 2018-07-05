#pragma once
#include "CSoundPlayer.h"
#include <string>

typedef struct {
	std::wstring* Title;
	std::wstring* Artist;
	std::wstring* Album;
	std::wstring* Time;
}MP3Info;

extern "C" bool __declspec(dllexport) GetMP3Info(LPWSTR file, MP3Info **outInfo, BOOL isWav);
extern "C" __declspec(dllexport) MP3Info*  GetMusicInfo(LPWSTR file, BOOL isWav);
extern "C" void __declspec(dllexport) DestroyMusicInfo(MP3Info* lastInfo);

//创建 播放器实例
//*你的程序主窗口句柄
CSoundPlayer * CreateIPlayer(HWND hWnd);
//释放 播放器实例
// * 请在程序退出时使用此方法
// * iplayer ：播放器实例
void DestroyIPlayer(CSoundPlayer*iplayer);

//关闭已打开的音乐
extern "C" BOOL __declspec(dllexport) CloseMusic();
/*打开音乐
* file ：音乐路径
*/
extern "C" BOOL __declspec(dllexport) OpenMusic(LPWSTR file);
/*停止音乐*/
extern "C" BOOL __declspec(dllexport) StopMusic();
/*
播放或暂停音乐
* 如果音乐正在播放，则暂停音乐；如果音乐暂停，则开始播放音乐（相当于播放/暂停按钮）。
*/
extern "C" BOOL __declspec(dllexport) PausePlayMusic();
/*初始化播放器
* 调用本库的C语言api时，请先初始化播放器
* hWnd ：调用者窗口句柄
*/
extern "C" BOOL __declspec(dllexport) IPlayerInit(HWND hWnd);
/*从头开始播放当前音乐*/
extern "C" BOOL __declspec(dllexport) ReplayMusic();
/*开始播放音乐*/
extern "C" BOOL __declspec(dllexport) PlayMusic();
/*暂停播放当前音乐*/
extern "C" BOOL __declspec(dllexport) PauseMusic();
//获取当前音乐的长度
// * 以 秒 为单位
extern "C" double __declspec(dllexport) GetMusicLength();
/*设置当前音乐播放位置
* val：当前音乐播放位置，单位为 秒
*/
extern "C" BOOL __declspec(dllexport) SetMusicPos(double val);
/*获取当前音乐播放位置
* 返回：当前音乐播放位置，单位为 秒
*/
extern "C" double __declspec(dllexport) GetMusicPos();
/*获取音乐是否已打开*/
extern "C" BOOL __declspec(dllexport) GetOpened();
/*获取音乐时长字符串
* 格式是：00:00 音乐时长
*/
extern "C" LPWSTR __declspec(dllexport) GetMusicLenTimeString();
/*获取音乐正在播放的时间字符串
* 格式是：00:00 正在播放时间
*/
extern "C" LPWSTR __declspec(dllexport) GetMusicPosTimeString();
/*获取音乐正在播放的时间字符串
* 格式是：00:00/00:00 正在播放时间/音乐时长
*/
extern "C" LPWSTR __declspec(dllexport) GetMusicTimeString();
/*获取播放器上一个错误信息*/
extern "C" LPWSTR __declspec(dllexport) GetPlayerError();

//获取音乐播放的样本位置
// * 以 样本 为单位
// * 使用 GetMusicSampleRate 获取 音乐采样率
extern "C" DWORD __declspec(dllexport) GetMusicPosSample();
//设置音乐播放的位置
// * 以 样本 sample 为单位
extern "C" DWORD __declspec(dllexport) SetMusicPosSample(DWORD sample);

//获取音乐文件的 采样率
extern "C" DWORD __declspec(dllexport) GetMusicSampleRate();
//获取音乐文件的 量化位数 （8/16/32）
extern "C" int __declspec(dllexport) GetMusicBitPerSample();
//获取音乐文件的 声道数
extern "C" int __declspec(dllexport) GetMusicChannelsCount();
//获取音乐文件的 长度
// * 以 样本 sample 为单位
extern "C" DWORD __declspec(dllexport) GetMusicLengthSample();

/*销毁播放器
* 请在程序退出时使用此方法
*/
extern "C" BOOL __declspec(dllexport) IPlayerDestroy();
/*音乐淡出（2s）
* currvol ：当前音量
* 相当于音量慢慢变小
*/
extern "C" BOOL __declspec(dllexport) MusicTweedOut(int currvol);
/*音乐淡入（2s）
* currvol ：当前音量
* 相当于音量慢慢变大
*/
extern "C" BOOL __declspec(dllexport) MusicTweedIn(int currvol);
/**/
extern "C" BOOL __declspec(dllexport) SetFFTHDC(HDC hdc);
/**/
extern "C" BOOL __declspec(dllexport) DrawFFTOnHDC(HDC hdc);
/**/
extern "C" BOOL __declspec(dllexport) DrawFFTOnHDCSmall(HDC hdc);
/*获取播放器状态
* TPlayerStatus::Unknow,
* TPlayerStatus::NotOpen, 未打开文件
* TPlayerStatus::Opened, 文件已打开就绪
* TPlayerStatus::Playing, 正在播放
* TPlayerStatus::Paused, 暂停中
* TPlayerStatus::PlayEnd, 播放到音乐结尾了
*/
extern "C" TPlayerStatus __declspec(dllexport) GetMusicPlayState();
/*获取播放器音量
* 返回音量数值 ：1-100
*/
extern "C" int __declspec(dllexport) GetVolume();
/*设置播放器音量
* vol：音量数值 （1-100）
*/
extern "C" BOOL __declspec(dllexport) SetVolume(int vol);
/*设置默认音频输出参数
* sample_rate 采样率
* channels 声道数
* bits_per_sample 采样位数
*/
extern "C" BOOL __declspec(dllexport) SetDefOutPutSettings(ULONG sample_rate, int channels, int bits_per_sample);
