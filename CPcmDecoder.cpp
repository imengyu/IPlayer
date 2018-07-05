#include "stdafx.h"
#include "CPcmDecoder.h"
#include "resource2.h"

CPcmDecoder::CPcmDecoder(IPlayer*parent)
{
	this->parent = parent;
}
CPcmDecoder::~CPcmDecoder()
{
	if (_Stream)
		Close();
}

bool CPcmDecoder::Open(LPWSTR file)
{
	_wfopen_s(&_Stream, file, L"rb");
	if (_Stream) {
		fseek(_Stream, 0L, SEEK_END);
		_FileSize = ftell(_Stream);
		fseek(_Stream, 0, SEEK_SET);
		_FileSec = (double)_FileSize / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);
		return true;
	}
	else err(L"Failed open file.");
	return false;
}
bool CPcmDecoder::Close()
{
	fclose(_Stream);
	_Stream = NULL;
	return true;
}

int CPcmDecoder::GetMusicChannelsCount()
{
	if (!parent->defoutseted() && !required_info) {
		if (!require_info())
			return -1;
		return channels;
	}
	else if (parent->defoutseted()) return parent->get_def_channels();
	else return channels;
}
int CPcmDecoder::GetMusicBitsPerSample()
{
	if (!parent->defoutseted() && !required_info) {
		if (!require_info())
			return -1;
		return bits_per_sample;
	}
	else if (parent->defoutseted()) return parent->get_def_bits_per_sample();
	else return bits_per_sample;
}
ULONG CPcmDecoder::GetMusicSampleRate()
{
	if (!parent->defoutseted() && !required_info) {
		if (!require_info())
			return -1;
		return sample_rate;
	}
	else if (parent->defoutseted()) return parent->get_def_sample_rate();
	else return sample_rate;
}

double CPcmDecoder::GetMusicLength()
{
	return _FileSec;
}
DWORD CPcmDecoder::GetMusicLengthSample()
{
	return 0;
}
DWORD CPcmDecoder::GetCurSample()
{
	long cur = ftell(_Stream);
	_CurSec = (double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);
	return  static_cast<DWORD>((double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample()) / 8));
}
DWORD CPcmDecoder::SeekToSample(DWORD sp)
{
	_CurSec = sp / GetMusicSampleRate();
	SeekToSec(_CurSec);
	return 0;
}
double CPcmDecoder::SeekToSec(double sec)
{
	_CurSec = sec;
	return static_cast<double>(fseek(_Stream, static_cast<LONG>((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8 * sec), SEEK_SET));
}

size_t CPcmDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	if (!_Stream)return -1;
	return fread_s(_Buffer, _BufferSize, 1, _BufferSize, _Stream);
}
double CPcmDecoder::GetCurSec()
{
	long cur = ftell(_Stream);
	_CurSec = (double)cur / ((double)(GetMusicChannelsCount()*GetMusicBitsPerSample() * GetMusicSampleRate()) / 8);
	return _CurSec;
}

bool CPcmDecoder::IsOpened()
{
	return _Stream != NULL;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern HINSTANCE hInst;
HWND hBTS2;
HWND hEdSp;
ULONG ched_sample_rate = 44100;
int ched_channels = 2;
int ched_bits_per_sample = 16;
HWND hSps;

bool CPcmDecoder::require_info()
{
	if (!required_info_canceld) {
		if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_CHOOSE_INFO), parent->hostHWnd, DlgProc) == IDOK)
		{
			sample_rate = ched_sample_rate;
			channels = ched_channels;
			bits_per_sample = ched_bits_per_sample;

			required_info = true;
			return true;
		}
		else required_info_canceld = true;
	}
	return false;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG: {
		HWND hBTS = GetDlgItem(hDlg, IDC_GROUP_BITPS);
		hBTS2 = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW_BITFS), hBTS, (DLGPROC)DlgProc);
		RECT rc; GetWindowRect(hBTS2, &rc);
		MoveWindow(hBTS2, 5, 20, rc.right - rc.left, rc.bottom - rc.top, FALSE);
		ShowWindow(hBTS2, SW_SHOWNA);

		hSps = GetDlgItem(hDlg, IDC_LIST_SAMPLERATES);
		hEdSp = GetDlgItem(hDlg, IDC_EDIT_SAMPLE_RATE);

		CheckDlgButton(hDlg, IDC_RADIO_16BIT, TRUE);
		CheckDlgButton(hDlg, IDC_RADIO_STREO, TRUE);
		SetDlgItemText(hDlg, IDC_EDIT_SAMPLE_RATE, L"44100");

		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"192000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"96000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"88200");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"64000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"48000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"44100");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"32000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"22050");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"16000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"11025");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"8000");
		SendMessage(hSps, LB_ADDSTRING, 0, (LPARAM)L"6000");

		return 0;
	}
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
			EndDialog(hDlg, IDCANCEL);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK: {
			TCHAR intbuf[32];
			GetDlgItemText(hDlg, IDC_EDIT_SAMPLE_RATE, intbuf, 32);
			if (wcscmp(intbuf, L"") == 0) {
				MessageBox(hDlg, L"请输入采样率", L"提示", MB_ICONEXCLAMATION);
				return TRUE;
			}
			ULONG sp = static_cast<ULONG>(_wtoll(intbuf));
			if (sp <= 0 || sp > 192000) {
				MessageBox(hDlg, L"您输入的采样率可能不对", L"提示", MB_ICONEXCLAMATION);
				return TRUE;
			}
			else ched_sample_rate = sp;

			if (IsDlgButtonChecked(hDlg, IDC_RADIO_SINGAL))
				ched_channels = 1;
			else ched_channels = 2;

			if (IsDlgButtonChecked(hDlg, IDC_RADIO_8BIT))
				ched_bits_per_sample = 8;
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO_16BIT))
				ched_bits_per_sample = 16;
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO_32BIT))
				ched_bits_per_sample = 32;
			else ched_bits_per_sample = 16;

			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_LIST_SAMPLERATES:
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE: {
				int lbItem = (int)SendMessage(hSps, LB_GETCURSEL, 0, 0);
				TCHAR buff[MAX_PATH];
				SendMessage(hSps, LB_GETTEXT, lbItem, (LPARAM)buff);
				SetDlgItemText(hDlg, IDC_EDIT_SAMPLE_RATE, buff);
				return TRUE;
			}
			}
		}
		return TRUE;
		}
	}
	return (INT_PTR)FALSE;
}
