#include "stdafx.h"
#include "CMidiDecoder.h"


CMidiDecoder::CMidiDecoder()
{
}
CMidiDecoder::~CMidiDecoder()
{
	if (IsOpened())
		Close();
}

DWORD CMidiDecoder::getinfo(UINT wDeviceID, DWORD item)
{
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = NULL;
	mcistatusparms.dwItem = item;
	mcistatusparms.dwReturn = 0;
	mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcistatusparms);
	return mcistatusparms.dwReturn;
}
bool CMidiDecoder::Open(LPWSTR file)
{
	MCI_OPEN_PARMS OpenParms;
	OpenParms.lpstrDeviceType = (LPWSTR)MCI_DEVTYPE_SEQUENCER;//MIDI类型
	OpenParms.lpstrElementName = file;
	OpenParms.wDeviceID = 0;
	MCIERROR rs = mciSendCommand(NULL, MCI_OPEN,
		MCI_WAIT | MCI_OPEN_TYPE |
		MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT,
		(DWORD)(LPVOID)&OpenParms);
	if (rs == 0)
	{
		MCI_SET_PARMS mciSet;
		mciSet.dwTimeFormat = MCI_FORMAT_MILLISECONDS; //设置时间单位为毫秒  
		mciSendCommand(OpenParms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&mciSet);

		openedmidi = OpenParms.wDeviceID;

		MCI_PLAY_PARMS pp;
		pp.dwCallback = NULL;
		pp.dwFrom = 0;
		mciSendCommand(openedmidi, MCI_PLAY, MCI_NOTIFY, (DWORD)&pp);

		double alltime = getinfo(openedmidi, MCI_STATUS_LENGTH) / 1000;
		m_LengthSec = alltime;
		m_Opned = true;

		return true;
	}
	else err(L"MCI open failed!");
	return false;
}
bool CMidiDecoder::Close()
{
	if (m_Opned) {
		mciSendCommand(openedmidi, MCI_CLOSE, NULL, NULL);
		m_Opned = false;
		m_LengthSec = 0;
	}
	return false;
}

int CMidiDecoder::GetMusicChannelsCount()
{
	return 0;
}
int CMidiDecoder::GetMusicBitsPerSample()
{
	return 0;
}
int CMidiDecoder::GetMusicSampleRate()
{
	return 0;
}
int CMidiDecoder::GetMusicBitrate()
{
	return 0;
}

double CMidiDecoder::GetMusicLength()
{
	return m_LengthSec;
}
double CMidiDecoder::GetCurSample()
{
	return 0.0;
}
double CMidiDecoder::SeekToSample(double sec)
{
	return 0.0;
}
double CMidiDecoder::SeekToSec(double sec)
{
	double time = sec * m_LengthSec * 1000;
	MCI_SEEK_PARMS SeekParms;
	SeekParms.dwTo = static_cast<ULONG>(time);
	//跳转的目标时间，时间单位为毫秒
	mciSendCommand(openedmidi, MCI_SEEK, MCI_TO
		| MCI_WAIT, (DWORD)(LPVOID)
		&SeekParms);
	return 0.0;
}
size_t CMidiDecoder::Read(void * _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount)
{
	return size_t();
}
int CMidiDecoder::Seek(long _Offset, int _Origin)
{
	return 0;
}
double CMidiDecoder::GetCurSec()
{
	double thistime = getinfo(openedmidi, MCI_STATUS_POSITION) / 1000;
	return thistime;
}
bool CMidiDecoder::IsOpened()
{
	return m_Opned;
}
