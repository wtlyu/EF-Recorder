#include "videorecorder.h"

#include <windows.h>

#include <QDebug>
#include <QMessageBox>

#include "DeviceEnumerator.h"

VideoRecorder::VideoRecorder(QObject *parent) : QThread(parent)
{

}

QMap<int, QString> VideoRecorder::get_device_list()
{
	DeviceEnumerator de;

	QMap<int, QString> ret;

	// Devices
	std::map<int, Device> devices = de.getVideoDevicesMap();

	// Print information about the devices
	for (auto const &device : devices)
		ret.insert(device.first, QString::fromStdString(device.second.deviceName));

	return ret;
}

int VideoRecorder::select_device(QString name)
{
	if (name == "Default")
		return 0;

	QMap<int, QString> device_list = get_device_list();

	for (QMap<int, QString>::const_iterator i = device_list.constBegin(); i != device_list.constEnd(); ++i)
		if (i.value() == name)
			return i.key();

	return 0;
}

void VideoRecorder::set_device(QString name)
{
	device_id = select_device(name);
}

void VideoRecorder::on_timer_tick()
{
	capture >> frame;
	if (!frame.empty())
	{
		if (preview)
		{
			emit tick(frame);
		} else {
			if (writer.isOpened()) writer.write(frame);
		}
	}
}

bool VideoRecorder::init()
{
	bool ret;

	// init
	qDebug() << "[videoRecorder] init";
	qDebug() << "[videoRecorder] camera id: " << device_id;

	if (capture.isOpened())
		capture.release();     //decide if capture is already opened; if so,close it
	if (writer.isOpened())
		writer.release();     //decide if writer is already opened; if so,close it

	ret = capture.open(device_id);           //open the default camera
	if (!ret)
	{
		emit error("Video Recoder Filed: Failed to open camera device!");
		//QMessageBox::critical(this, tr("Video Recoder Filed..."), tr("Failed to open camera device!"));
		return false;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	capture.set(CV_CAP_PROP_FPS, rate);

	on_timer_tick();

	if (!preview)
	{
		ret = writer.open(filename.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), rate, cv::Size(frame.cols, frame.rows), true);
		if (!ret)
		{
			emit error("Video Recoder Filed: Failed to output file!");
			//QMessageBox::critical(this, "Video Recoder Filed...", "Failed to open output file!");
			return false;
		}

		SetFileAttributes((reinterpret_cast<const wchar_t *>(filename.utf16())), FILE_ATTRIBUTE_HIDDEN);
	}

	tick_last_time = QDateTime::currentDateTime();

	return true;
}

bool VideoRecorder::finish()
{
	bool ret;

	// finish
	qDebug() << "[videoRecorder] finish";

	if (capture.isOpened())
		capture.release();     //decide if capture is already opened; if so,close it
	if (writer.isOpened())
		writer.release();     //decide if writer is already opened; if so,close it

	return true;
}

void VideoRecorder::run()
{
	// init
	qDebug() << "[videoRecorder] start";

	while (1)
	{
		if (tick_last_time.msecsTo(QDateTime::currentDateTime()) >= 1000/rate)
		{
			on_timer_tick();
			tick_last_time = QDateTime::currentDateTime();
		}
		if (should_stop) break;
		msleep((int)(1000/rate/2));
	}
}
