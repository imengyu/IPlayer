#pragma once
#include "CSoundDecoder.h"
#include <mmsystem.h>

class IPlayer;
class CMidiDecoder :
	public CSoundDecoder
{
public:
	CMidiDecoder(IPlayer*parent);
	~CMidiDecoder();



	DWORD GetStatus() { return getinfo(openedmidi, MCI_STATUS_MODE); }
	bool Open(LPWSTR file) override;
	bool Close() override;

	double GetMusicLength()override;
	double SeekToSec(double sec)override;
	double GetCurSec();
	bool IsOpened() override;
	MCIDEVICEID GetMidiId() {
		return openedmidi;
	}
private:
	DWORD getinfo(UINT wDeviceID, DWORD item);

	IPlayer*parent;
	MCIDEVICEID openedmidi = 0;
	double m_LengthSec = 0;
	double m_CurSample = 0;
	bool m_Opned = false;
};

