#include "settings.h"

Settings::Settings()
{
	settings = new QSettings("EF", "Video Rocorder");
}

void Settings::restore()
{
	settings->clear();
	settings->sync();
}

QString Settings::get_root_dir()
{
	settings->sync();
	return settings->value("root_dir", "C:\\").toString();
}

QString Settings::get_school()
{
	settings->sync();
	return settings->value("school", "Untitled School").toString();
}

QTime Settings::get_max_duration()
{
	settings->sync();
	return settings->value("max_duration", QTime(0, 0, 30)).toTime();
}

QString Settings::get_video()
{
	settings->sync();
	return settings->value("video", "Default").toString();
}

QString Settings::get_audio()
{
	settings->sync();
	return settings->value("audio", "Default").toString();
}

QByteArray Settings::get_geometry()
{
	settings->sync();
	return settings->value("geometry").toByteArray();
}

void Settings::set_root_dir(QString root_dir)
{
	settings->setValue("root_dir", root_dir);
	settings->sync();
}

void Settings::set_school(QString school)
{
	settings->setValue("school", school);
	settings->sync();
}

void Settings::set_max_duration(QTime max_duration)
{
	settings->setValue("max_duration", max_duration);
	settings->sync();
}

void Settings::set_video(QString video)
{
	settings->setValue("video", video);
	settings->sync();
}

void Settings::set_audio(QString audio)
{
	settings->setValue("audio", audio);
	settings->sync();
}

void Settings::set_geometry(QByteArray geometry)
{
	settings->setValue("geometry", geometry);
	settings->sync();
}
