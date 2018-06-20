#pragma once
#include "CSoundDecoder.h"
#include <mmsystem.h>

class CMidiDecoder :
	public CSoundDecoder
{
public:
	CMidiDecoder();
	~CMidiDecoder();



	DWORD GetStatus() { return getinfo(openedmidi, MCI_STATUS_MODE); }
	bool Open(LPWSTR file) override;
	bool Close() override;


	int GetMusicChannelsCount()override;
	int GetMusicBitsPerSample()override;
	int GetMusicSampleRate()override;
	int GetMusicBitrate();
	double GetMusicLength()override;

	double GetCurSample()override;
	double SeekToSample(double sec)override;
	double SeekToSec(double sec)override;
	size_t Read(void*  _Buffer, size_t _BufferSize, size_t _ElementSize, size_t _ElementCount) override;
	int Seek(long  _Offset, int  _Origin) override;

	double GetCurSec();
	bool IsOpened() override;
	MCIDEVICEID GetMidiId() {
		return openedmidi;
	}
private:
	DWORD getinfo(UINT wDeviceID, DWORD item);

	MCIDEVICEID openedmidi = 0;
	double m_LengthSec = 0;
	double m_CurSample = 0;
	bool m_Opned = false;
};

