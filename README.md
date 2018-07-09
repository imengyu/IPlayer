# IPlayer
A simply sound decorder and player for windows, use directsound directsound 音频解码器<br/><br/>
这是一个最简单的适用于windows平台c++音频解码模块，使用DirectSound进行播放。
===
        目前支持播放 mp3、wav、ogg、wma、flac、oga、pcm 这几种音频格式，未来还会添加其他格式的支持。
说明
---
        此项目目前仅为测试阶段，所以最好不要用于发行软件中。软件可能存在bug。
使用
---
* 1.请#include "IPlayerExport.h"
* 2.添加链接器附加依赖项 iplayer.lib
* 3.添加代码
```cpp
CSoundPlayer *player=CreateIPlayer(m_hWnd);//Your app window handle
if(player->Open(L"E:\\1.mp3"))//open E:\1.mp3
{
    player->Play();
}
else//open failed ,show error
{
    wprintf(L"Error : %s", GetPlayerError());
}
```
其他使用方法，请参照 WindowsFormsTest 项目里的代码（c#）<br/>

其中使用了以下一些开源解码库
---
* libmpg123
* libogg
* libvorbis
* libFLAC
* libffad
* libffac

