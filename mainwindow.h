#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QDateTime>
#include <QMenu>
#include <QSystemTrayIcon>

#include "audiorecorder.h"
#include "videorecorder.h"
#include "videomuxing.h"
#include "configwindow.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

	void timeout();

	void on_error(QString msg);

private:
    Ui::MainWindow *ui;
	QMenu myMenu;
	QTimer *timer;
	QSystemTrayIcon *trayIcon;

	void mousePressEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void show_message(QString title, QString msg, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information);

	void show_menu();

	void start_rec();

	void stop_rec();

	void triger();

	void open_settings();

	QPoint dragPosition;
	bool moved = false;

	AudioRecorder *audioRecorder;
	VideoRecorder *videoRecorder;
	VideoMuxing *videoMuxing;

	int recording_status = 0;

	ConfigWindow *configwindow;

	Settings settings;
};

#endif // MAINWINDOW_H
