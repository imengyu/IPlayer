#include "stdafx.h"
#include "CFLACDecoder.h"
#include "CStringConv.h"


CFLACDecoder::CFLACDecoder(bool ogg)
{
	isOgg = ogg;
	//hEventDecorderFinish = CreateEvent(0, 0, 0, 0);
}
CFLACDecoder::~CFLACDecoder()
{
	//CloseHandle(hEventDecorderFinish);
	if (IsOpened())
		Close();
}

void CFLACDecoder::CFLACDecoder_FLAC__StreamDecoderErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *data)
{
	CFLACDecoder *instance = static_cast<CFLACDecoder *>(data);
	switch (status)
	{
	    case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:		
			instance->err(L"lose synchronization");
			break;
	    case	FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:		
			instance->err(L"BAD_HEADER");
			break;
	    case	FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
			//instance->err(L"BAD_HEADER");
		    break;
	    case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
			instance->err(L"BAD_FORMAT");
			break;
		default:
			break;
	}
}
FLAC__StreamDecoderWriteStatus CFLACDecoder::CFLACDecoder_FLAC__StreamDecoderWriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *data)
{
	CFLACDecoder *instance = static_cast<CFLACDecoder *>(data);

	if (instance->bit_per_sample)
	{
		if (instance->bit_per_sample != frame->header.bits_per_sample)
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
	}
	else instance->bit_per_sample = frame->header.bits_per_sample;
	if (instance->sample_rate)
	{
		if (instance->sample_rate != frame->header.sample_rate)
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;	// skip this frame	
	}
	else instance->sample_rate = frame->header.sample_rate;
	if (instance->channel)
	{
		if (instance->channel != frame->header.channels)
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;	// skip this frame	
	}
	else instance->channel = frame->header.channels;

	unsigned int right = 0;
	if (frame->header.channels==2) right = 1;
	else if (frame->header.channels == 2) right = 0;
	else return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT; // multiple channels are not supported

	// do work here
	switch (frame->header.bits_per_sample)
	{
	case 24: {
		// copy data from decoder into buffer
		unsigned int i;
		unsigned int j;
		short *sample = (short*)instance->buffer_cache2;
		for (i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
		{
			sample[j] = buffer[0][i] >> 8;
			sample[j + 1] = buffer[right][i] >> 8;
		}
		break;
	}
	case 16: {
		// copy data from decoder into buffer
		unsigned int i;
		unsigned int j;
		short *sample = (short*)instance->buffer_cache2;

		for (i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
		{
			sample[j] = buffer[0][i];
			sample[j + 1] = buffer[right][i];
		}
		break;
	}
	case 8: {
		// copy data from decoder into buffer
		unsigned int i;
		unsigned int j;
		short *sample = (short*)instance->buffer_cache2;
		for (i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
		{
			sample[j] = buffer[0][i] << 8;
			sample[j + 1] = buffer[right][i] << 8;
		}
		break;
	}
	default: return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	UINT readbytes = (frame->header.blocksize *  frame->header.bits_per_sample* frame->header.channels) / 8;

	memcpy_s(instance->buffer_cache + instance->readed_bytes, sizeof(instance->buffer_cache) - instance->readed_bytes,
		instance->buffer_cache2, readbytes);
	memset(instance->buffer_cache2, 0, sizeof(instance->buffer_cache2));

	instance->readed_bytes += readbytes;
	instance->curPos += frame->header.blocksize;
	instance->curosSec = instance->curPos / (double)frame->header.sample_rate;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
void CFLACDecoder::CFLACDecoder_FLAC__StreamDecoderMetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *data)
{
	CFLACDecoder *instance = static_cast<CFLACDecoder *>(data);
	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		instance->sample_rate = metadata->data.stream_info.sample_rate;
		instance->channel = metadata->data.stream_info.channels;
		instance->bit_per_sample = metadata->data.stream_info.bits_per_sample;
		instance->lengthSample = metadata->data.stream_info.total_samples;
		instance->lengthSec = instance->lengthSample / (double)(instance->sample_rate);
		instance->bfs = instance->sample_rate * static_cast<ULONG>(instance->channel) * static_cast<ULONG>(instance->bit_per_sample) / 8;
		instance->sps = instance->channel * instance->bit_per_sample / 8;
		if (instance->sample_rate <= 0 || instance->sample_rate > 2) instance->err(L"Unsupport channel count");
	}
	else instance->err(L"Unknow error.");
}

bool CFLACDecoder::Open(LPWSTR file)
{
	pDecoder = FLAC__stream_decoder_new();
	if (!pDecoder)
		return err(L"Failed init FLAC decoder!\n");
	LPCSTR str = CStringConv::w2a(file);
	FLAC__StreamDecoderInitStatus st;
	if (isOgg) st = FLAC__stream_decoder_init_ogg_file(pDecoder, str,
			CFLACDecoder_FLAC__StreamDecoderWriteCallback,
			CFLACDecoder_FLAC__StreamDecoderMetadataCallback,
			CFLACDecoder_FLAC__StreamDecoderErrorCallback,
			this);
	else st = FLAC__stream_decoder_init_file(pDecoder, str,
		CFLACDecoder_FLAC__StreamDecoderWriteCallback,
		CFLACDecoder_FLAC__StreamDecoderMetadataCallback,
		CFLACDecoder_FLAC__StreamDecoderErrorCallback,
		this);
	CStringConv::del(str);
	if (st == FLAC__STREAM_DECODER_INIT_STATUS_OK) {
		// decode metadata	
		curPos = 0;
		if (FLAC__stream_decoder_process_until_end_of_metadata(pDecoder) == 0)
		{
			// decode metadata
			if (FLAC__stream_decoder_process_until_end_of_metadata(pDecoder) == 0)
				return err(L"Bad format.");
		}
		opened = true;
		return opened;
	}
	else if (st == FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR)
		return err(L"ÄÚ´æ²»×ã");
	else if (st == FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE)
		return err(L"Failed open file");
	return false;
}
bool CFLACDecoder::Close()
{
	if (IsOpened())
	{
		FLAC__stream_decoder_delete(pDecoder);
		opened = false;
		return true;
	}
	return false;
}

int CFLACDecoder::GetMusicChannelsCount()
{
	return channel;
}
int CFLACDecoder::GetMusicBitsPerSample()
{
	return bit_per_sample;
}
DWORD CFLACDecoder::GetMusicSampleRate()
{
	return sample_rate;
}

double CFLACDecoder::GetMusicLength()
{
	return lengthSec;
}

double CFLACDecoder::SeekToSec(double sec)
{
	FLAC__uint64 nSamples = static_cast<FLAC__uint64>(sec * sample_rate);
	curPos = nSamples;
	if (FLAC__stream_decoder_seek_absolute(pDecoder, nSamples) == false)
		return err(L"FLAC UNKNOW ERROR");
	if (FLAC__stream_decoder_get_state(pDecoder) == FLAC__STREAM_DECODER_SEEK_ERROR)
	{
		if (FLAC__stream_decoder_flush(pDecoder) == false)
			return err(L"FLAC UNKNOW ERROR");
		if (FLAC__stream_decoder_reset(pDecoder) == false)
			return err(L"FLAC UNKNOW ERROR");
	}
	return sec;
}
double CFLACDecoder::GetCurSec()
{
	return curosSec;
}

DWORD CFLACDecoder::GetMusicLengthSample()
{
	return static_cast<DWORD>(lengthSample);
}
DWORD CFLACDecoder::GetCurSample()
{
	return static_cast<DWORD>(curPos);
}
DWORD CFLACDecoder::SeekToSample(DWORD sp)
{
	FLAC__uint64 nSamples = static_cast<FLAC__uint64>(sp);
	curPos = nSamples;
	if (FLAC__stream_decoder_seek_absolute(pDecoder, nSamples) == false)
		return err(L"FLAC UNKNOW ERROR");
	if (FLAC__stream_decoder_get_state(pDecoder) == FLAC__STREAM_DECODER_SEEK_ERROR)
	{
		if (FLAC__stream_decoder_flush(pDecoder) == false)
			return err(L"FLAC UNKNOW ERROR");
		if (FLAC__stream_decoder_reset(pDecoder) == false)
			return err(L"FLAC UNKNOW ERROR");
	}
	return sp;
}

size_t CFLACDecoder::Read(void * _Buffer, size_t _BufferSize)
{
	while (readed_bytes < _BufferSize)
	{
		if (curPos >= lengthSample)
			return 0;
		if (!FLAC__stream_decoder_process_single(pDecoder))
			return 0;
	}
	size_t outSize = readed_bytes - _BufferSize;
	memcpy_s(_Buffer, _BufferSize, buffer_cache, _BufferSize);
	if (outSize > 0) {
		//copy to 2 > set 1 to 0
		//2 copy to1
		memcpy_s(buffer_cache2, sizeof(buffer_cache2), buffer_cache + _BufferSize, outSize);
		memset(buffer_cache, 0, sizeof(buffer_cache));
		memcpy_s(buffer_cache, sizeof(buffer_cache), buffer_cache2, outSize);
		memset(buffer_cache2, 0, sizeof(buffer_cache2));
		readed_bytes = outSize;
		return _BufferSize;
	}
	else {
		size_t o = readed_bytes;
		readed_bytes = 0;
		return o;
	}
}

bool CFLACDecoder::IsOpened()
{
	return opened;
}
