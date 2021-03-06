// BellsPlayer.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "IPlayerExport.h"
#include "IPlayer.h"
#include "PathHelper.h"
#include "DirectoryHelper.h"
#import "Shell32.dll"

using namespace std;

CSoundPlayer*player = NULL;
Shell32::IShellDispatchPtr ptrShell;



CSoundPlayer * CreateIPlayer(HWND hWnd)
{
	CoInitialize(NULL);
	ptrShell.CreateInstance(__uuidof(Shell32::Shell));
	return new IPlayer(hWnd);
}
void DestroyIPlayer(CSoundPlayer*iplayer)
{
	ptrShell.Release();
	CoUninitialize();
	delete iplayer;
}

#pragma region MP3 info Handler

bool GetMP3Info(LPWSTR file, MP3Info **outInfo, BOOL isWav)
{
	if (_waccess(file,0)==0) {
		std::wstring * dir = Path::GetDirectoryName(file);
		if (Directory::Exists(dir)) {

			MP3Info *mp3info = (MP3Info*)malloc(sizeof(MP3Info));
			std::wstring * name = Path::GetFileName(file);
			Shell32::FolderPtr ptrFolder = ptrShell->NameSpace(dir->c_str());
			Shell32::FolderItemPtr ptrItem = ptrFolder->ParseName(name->c_str());

			int timecol = 27, abcol = 14, singercol = 13, titlecol = 21;
			/*if (!mainAppLoader->IsSystemWin7Up())
			{
				timecol = 21;
				abcol = 17;
				singercol = 16;
				titlecol = 10;
			}*/

			wstring *strLength = new wstring((LPWSTR)ptrFolder->GetDetailsOf(_variant_t((IDispatch *)ptrItem), timecol));

			if (((*strLength)[0] == L'0' && (*strLength)[1] == L'0' && (*strLength)[2] == L':'))
				*strLength = strLength->substr(3, strLength->size() - 3);
			else if (((*strLength)[0] == L'0' && (*strLength)[1] == L':'))
				*strLength = strLength->substr(3, strLength->size() - 2);

			mp3info->Album = new wstring((LPWSTR)ptrFolder->GetDetailsOf(_variant_t((IDispatch *)ptrItem), abcol));
			mp3info->Title = new wstring((LPWSTR)ptrFolder->GetDetailsOf(_variant_t((IDispatch *)ptrItem), titlecol));
			mp3info->Artist = new wstring((LPWSTR)ptrFolder->GetDetailsOf(_variant_t((IDispatch *)ptrItem), singercol));
			mp3info->Time = strLength;

			ptrItem.Release();
			ptrFolder.Release();

			delete(dir);

			if (outInfo) {
				*outInfo = mp3info;
				return true;
			}
			else {
				delete mp3info;
				return false;
			}
		}
		delete(dir);
	}
	return false;
}
MP3Info* GetMusicInfo(LPWSTR file, BOOL isWav)
{
	MP3Info *info;
	if (GetMP3Info(file, &info, isWav))
		return info;
	return NULL;
}
void DestroyMusicInfo(MP3Info* lastInfo)
{
	if (lastInfo->Album)delete lastInfo->Album;
	if (lastInfo->Artist)delete lastInfo->Artist;
	if (lastInfo->Time)delete lastInfo->Time;
	if (lastInfo->Title)delete lastInfo->Title;
	if (lastInfo)free(lastInfo);
}

#pragma endregion

void Copyright2018ByDfish() {
	MessageBox(0, L"Copyright © 2018 By Dfish 版权所有", L"版权信息", MB_OK);
}
extern "C" BOOL __declspec(dllexport) OpenMusic(LPWSTR file)
{
	if (player)return player->Open(file);
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) CloseMusic()
{
	if (player)return player->Close();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) StopMusic()
{
	if (player)return player->Stop();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) PausePlayMusic()
{
	if (player)return player->PausePlay();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) IPlayerInit(HWND hWnd)
{
	BOOL rs = false;
	if (!player) {
		player = CreateIPlayer(hWnd);
		if (player) rs = true;
	}
	return rs;
}
extern "C" BOOL __declspec(dllexport) ReplayMusic()
{
	if (player) return player->Restart();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) PlayMusic()
{
	if (player) return player->Play();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) PauseMusic()
{
	if (player) return player->Pause();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) SetMusicPos(double val)
{
	if (player) return player->SetMusicPos(val);
	else return FALSE;
}
extern "C" double __declspec(dllexport) GetMusicLength()
{
	if (player) return player->GetMusicLength();
	else return FALSE;
}
extern "C" double __declspec(dllexport) GetMusicPos()
{
	if (player) return player->GetMusicPos();
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) GetOpened()
{
	if (player) return player->IsOpened();
	else return FALSE;
}
extern "C" LPWSTR __declspec(dllexport) GetMusicLenTimeString() {
	if (player) return player->GetMusicLenTimeString();
	else return NULL;
}
extern "C" LPWSTR __declspec(dllexport) GetMusicPosTimeString() {
	if (player) return player->GetMusicPosTimeString();
	else return NULL;
}
extern "C" LPWSTR __declspec(dllexport) GetMusicTimeString()
{
	if (player)
		return player->GetMusicTimeString();
	else return L"Not ceate.";
}
extern "C" LPWSTR __declspec(dllexport) GetPlayerError()
{
	if (player)
		return player->GetLastErr();
	else return L"Not ceate.";
}
extern "C" BOOL __declspec(dllexport) IPlayerDestroy()
{
	if (player) {
		DestroyIPlayer(player);
		return 1;
	}
	player = NULL;
	return FALSE;
}
extern "C" BOOL __declspec(dllexport) MusicTweedOut(int currvol)
{
	BOOL rs = false;
	if (player&&player->IsOpened()) {
		//TStreamTime pTimeEnd;
		//pTimeEnd.sec = playerPos.sec + 2;
		//player->GetPlayerVolume(&ol, &or );
		//rs = player->SlideVolume(tfSecond, &playerPos, ol, or , tfSecond, &pTimeEnd, 0, 0);
		player->FadeOut(2, currvol, 0);
	}
	return rs;
}
extern "C" BOOL __declspec(dllexport) MusicTweedIn(int currvol)
{
	BOOL rs = false;
	if (player&&player->IsOpened()) {
		//TStreamTime pTimeEnd;
		//pTimeEnd.sec = playerPos.sec + 2;
		//rs = player->SlideVolume(tfSecond, &playerPos, 0, 0, tfSecond, &pTimeEnd, ol, or);
		player->FadeIn(2, 0, currvol);
	}
	return rs;
}
extern "C" BOOL __declspec(dllexport) SetFFTHDC(HDC hdc) {
	BOOL rs = false;
	if (player)
	{
		player->SetFFTHDC(hdc);
	}
	return rs;
}
extern "C" BOOL __declspec(dllexport) DrawFFTOnHDC(HDC hdc)
{
	BOOL rs = false;
	if (player)
	{
		player->DrawFFTOnHDC(hdc);
	}
	return rs;
}
extern "C" BOOL __declspec(dllexport) DrawFFTOnHDCSmall(HDC hdc)
{
	BOOL rs = false;
	if (player)
	{
		player->DrawFFTOnHDCSmall(hdc);
	}
	return rs;
}
extern "C" TPlayerStatus __declspec(dllexport) GetMusicPlayState()
{
	if (player)
		return player->GetPlayerStatus();
	else return TPlayerStatus::Unknow;
}
extern "C" int __declspec(dllexport) GetVolume()
{
	if (player)
		return player->GetPlayerVolume();
	else return -1;
}
extern "C" BOOL __declspec(dllexport) SetVolume(int vol)
{
	if (player) {
		player->SetPlayerVolume(vol);
		return TRUE;
	}
	else return FALSE;
}
extern "C" BOOL __declspec(dllexport) SetDefOutPutSettings(ULONG sample_rate, int channels, int bits_per_sample)
{
	BOOL rs = false;
	if (player) {
		player->SetDefOutPutSettings(sample_rate, channels, bits_per_sample);
		rs = true;
	}
	return rs;
}
extern "C" DWORD __declspec(dllexport) GetMusicPosSample() {
	if (player)
		return player->GetMusicPosSample();
	return 0;
}
extern "C" DWORD __declspec(dllexport) SetMusicPosSample(DWORD sample) {
	if (player)
		return player->SetMusicPosSample(sample);
	return 0;
}
extern "C" DWORD __declspec(dllexport) GetMusicSampleRate() {
	if (player)
		return player->GetMusicSampleRate();
	return 0;
}
extern "C" int __declspec(dllexport) GetMusicBitPerSample() {
	if (player)
		return player->GetMusicBitPerSample();
	return 0;
}
extern "C" int __declspec(dllexport) GetMusicChannelsCount()
{
	if (player)
		return player->GetMusicChannelsCount();
	return 0;
}
extern "C" DWORD __declspec(dllexport) GetMusicLengthSample()
{
	if (player)
		return player->GetMusicLengthSample();
	return 0;
}
