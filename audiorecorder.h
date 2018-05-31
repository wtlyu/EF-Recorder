#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <iostream>
#include <cstdio>

#include <QObject>
#include <QString>
#include <QMap>

#include "bass.h"
#include "basswma.h"

using namespace std;

class AudioRecorder : public QObject
{
	Q_OBJECT
public:
	explicit AudioRecorder(QObject *parent = nullptr);

	QMap<int, QString> get_device_list();

	int select_device(QString name);

	void set_device(QString name);

	bool init();

	bool free();

	bool start();

	int stop();

	int device_id = 0;
	QString filename;

private:

    HRECORD record;
    FILE *file;
	HWMENCODE wma_handler;

signals:
	void error(QString msg);

};

#endif // AUDIORECORDER_H
