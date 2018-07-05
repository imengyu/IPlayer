#include "stdafx.h"
#include "CMidiDecoder.h"
#include "IPlayer.h"

CMidiDecoder::CMidiDecoder(IPlayer*parent)
{
	this->parent = parent;
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


double CMidiDecoder::GetMusicLength()
{
	return m_LengthSec;
}
double CMidiDecoder::SeekToSec(double sec)
{
	DWORD time = (DWORD)((double)sec * (double)m_LengthSec) * 1000;
	parent->Pause();
	MCI_PLAY_PARMS pp;
	pp.dwFrom = time;
	mciSendCommand(openedmidi, MCI_PLAY, MCI_FROM, (DWORD)&pp);
	return time;
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
