#ifndef VIDEOMUXING_H
#define VIDEOMUXING_H

#include <QObject>
#include <QString>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat\avformat.h"
#ifdef __cplusplus
}
#endif

class VideoMuxing : public QObject
{
	Q_OBJECT
public:
	explicit VideoMuxing(QObject *parent = nullptr);

	bool process();

	QString vfilename;
	QString afilename;
	QString vfilename2;
	QString afilename2;
	QString filename;

private:
	int64_t get_file_duration(QString filename);

	int64_t get_audio_duration();
	int64_t get_video_duration();

signals:
	void error(QString msg);

};

#endif // VIDEOMUXING_H
