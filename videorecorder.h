#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QThread>
#include <QString>
#include <QTime>
#include <QMap>

#include "opencv2/opencv.hpp"

class VideoRecorder : public QThread
{
    Q_OBJECT
public:
	explicit VideoRecorder(QObject *parent = nullptr);

	QMap<int, QString> get_device_list();

	int select_device(QString name);

	void set_device(QString name);

	bool init();

	bool finish();

	int device_id = 0;
	bool should_stop = false;
	bool preview = false;
	QString filename;

	int width = 640;
	int height = 360;

private:
	void run() override;

	void on_timer_tick();

	cv::Mat frame;
	cv::VideoCapture capture;
   // QImage  image;
	//QTimer *timer;
	double rate = 30; //FPS
	cv::VideoWriter writer;   //make a video record
	QDateTime tick_last_time = QDateTime::fromTime_t(0);

signals:
	void tick(cv::Mat frame);

	void error(QString msg);

public slots:
};

#endif // VIDEORECORDER_H
