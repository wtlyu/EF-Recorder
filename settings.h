#ifndef SETTINGS_H
#define SETTINGS_H

#include <qsettings.h>
#include <QString>
#include <QTime>

class Settings
{
public:
    Settings();

	void restore();

	QString get_root_dir();

	QString get_school();

	QTime get_max_duration();

	QString get_video();

	QString get_audio();

	QByteArray get_geometry();

	void set_root_dir(QString root_dir);

	void set_school(QString school);

	void set_max_duration(QTime max_duration);

	void set_video(QString video);

	void set_audio(QString audio);

	void set_geometry(QByteArray geometry);

private:
	QSettings *settings;
};

#endif // SETTINGS_H
