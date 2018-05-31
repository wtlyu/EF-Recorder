#include "audiorecorder.h"

#include <windows.h>

#include <QDebug>
#include <QTime>

#include "DeviceEnumerator.h"

QTime t;
BOOL CALLBACK MyRecordingWriter(HRECORD handle, const void *buf, DWORD len, void *user)
{
	HWMENCODE *wma_handler = (HWMENCODE *)user;
	BASS_WMA_EncodeWrite(*wma_handler, buf, len); // write the buffer to the file
    return TRUE; // continue recording
}

/*******************************************************************/
AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent)
{

}

QMap<int, QString> AudioRecorder::get_device_list()
{
	QMap<int, QString> ret;

	int i, count=0;
	BASS_DEVICEINFO info;
	for (i=1; BASS_GetDeviceInfo(i, &info); i++)
		if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
		{
			ret.insert(i, QString::fromLocal8Bit(info.name));
			count++; // count it
		}

	return ret;
}

int AudioRecorder::select_device(QString name)
{
	if (name == "Default")
		return -1;

	QMap<int, QString> device_list = get_device_list();

	for (QMap<int, QString>::const_iterator i = device_list.constBegin(); i != device_list.constEnd(); ++i)
		if (i.value() == name)
			return i.key();

	return -1;
}

void AudioRecorder::set_device(QString name)
{
	device_id = select_device(name);
}

bool AudioRecorder::init()
{
	// init
	qDebug() << "[audioRecorder] init";
	qDebug() << "[audioRecorder] device id: " << device_id;

	bool ret = BASS_RecordInit(device_id);
	if (!ret) {
		int error_code = BASS_ErrorGetCode();
		switch (error_code) {
		case BASS_ERROR_DX:
			emit error("Audio Recoder Filed: DirectX (or ALSA on Linux or OpenSL ES on Android) is not installed.");
			break;
		case BASS_ERROR_DEVICE:
			emit error("Audio Recoder Filed: device is invalid.");
			break;
		case BASS_ERROR_ALREADY:
			//emit error("Audio Recoder Filed: The device has already been initialized.");
			return true;
			break;
		case BASS_ERROR_DRIVER:
			emit error("Audio Recoder Filed: There is no available device driver.");
			break;
		case BASS_ERROR_FORMAT:
			emit error("Audio Recoder Filed: The specified format is not supported by the device.");
			break;
		case BASS_ERROR_MEM:
			emit error("Audio Recoder Filed: There is insufficient memory.");
			break;
		case BASS_ERROR_NO3D:
			emit error("Audio Recoder Filed: Could not initialize 3D support.");
			break;
		case BASS_ERROR_UNKNOWN:
			emit error("Audio Recoder Filed: Some other mystery problem.");
			break;
		}
	}

	return ret;
}

bool AudioRecorder::free()
{
	// init
	qDebug() << "[audioRecorder] free";

	return BASS_Free();
}

bool AudioRecorder::start()
{
	// init
	qDebug() << "[audioRecorder] start";

	//t = QTime::currentTime();
	wma_handler = BASS_WMA_EncodeOpenFile(44100, 2, 0, 128000, filename.toLocal8Bit());
	if (!wma_handler)
	{
		emit error("Audio Recoder Filed: Failed to output file!");
		return false;
	}

	record = BASS_RecordStart(44100, 2, 0, MyRecordingWriter, &wma_handler); // start recording
	if (!record)
	{
		emit error("Audio Recoder Filed: Failed to open device!");
		return false;
	}

	return true;
}


int AudioRecorder::stop(void)
{
	// init
	qDebug() << "[audioRecorder] stop";

   // qDebug() <<  t.msecsTo(QTime::currentTime());
	BASS_ChannelStop(record);
	BASS_WMA_EncodeClose(wma_handler);

	SetFileAttributes((reinterpret_cast<const wchar_t *>(filename.utf16())), FILE_ATTRIBUTE_HIDDEN);

	//fclose(file);
}

