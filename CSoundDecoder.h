#pragma once
#include "stdafx.h"

//解码器
class CSoundDecoder
{
public:
	CSoundDecoder() {}
	virtual ~CSoundDecoder() {}

	//打开文件
	virtual bool Open(LPWSTR file) { return false; }
	//关闭文件
	virtual bool Close() { return false; }

	//获取音乐声道数
	virtual int GetMusicChannelsCount() { return 0; }
	//获取音乐量化位数
	virtual int GetMusicBitsPerSample() { return 0; }
	//获取音乐采样频率
	virtual ULONG GetMusicSampleRate() { return 0; }
	//获取音乐时长（秒）
	virtual double GetMusicLength() { return 0; }
	//获取音乐时长（sample）
	virtual DWORD GetMusicLengthSample() { return 0; }
	//获取音乐比特率（没有完全支持）
	virtual int GetMusicBitrate() { return 0; }

	//获取当前解码器解码位置（sample）
	virtual DWORD GetCurSample() { return 0; }
	//设置当前解码器解码位置（sample）
	virtual DWORD SeekToSample(DWORD sp) { return 0; }
	//获取当前解码器解码位置（秒）
	virtual double GetCurSec() { return 0; }
	//设置当前解码器解码位置（秒）
	virtual double SeekToSec(double sec) { return 0; }

	//解码并填充已解码的PCM数据至_Buffer中
	// * _Buffer 数据接收缓冲区
	// * _BufferSize 需要的数据大小，_Buffer大小必须>=_BufferSize
	// * 返回 已经填充的数据大小
	//    如果返回<_BufferSize或==0 ，GetLastErr()==L"" 说明已经播放完成
	virtual size_t Read(void*  _Buffer, size_t _BufferSize) { return 0; }

	//获取释放已经打开文件
	virtual bool IsOpened() { return false; }

	//释放
	virtual void Release() { delete this; }

	//设置错误
	bool err(wchar_t const* errmsg);
	//设置错误
	bool err(char const* errmsg);

	//获取上次错误
	LPWSTR GetLastErr() {
		return lasterr;
	}

	bool CreateInPlugin = false;
private:
	WCHAR lasterr[256];

};

