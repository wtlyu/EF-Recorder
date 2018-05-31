#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
#include <iostream>

#include <qDebug>
#include <QLocale>
#include <io.h>
#include <direct.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	audioRecorder = new AudioRecorder();
	videoRecorder = new VideoRecorder();
	videoMuxing = new VideoMuxing();
	configwindow = new ConfigWindow(this, audioRecorder, videoRecorder);
	timer = new QTimer(this);

	trayIcon = new QSystemTrayIcon(QIcon(QPixmap(":/img/EF-logo.png")));

	this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	this->setAttribute(Qt::WA_TranslucentBackground);

	myMenu.addAction("Status: Idle");
	myMenu.addSeparator();
	myMenu.addAction("Start/Stop");
	myMenu.addAction("Settings");
	myMenu.addSeparator();
	myMenu.addAction("Quit");
	myMenu.actions()[0]->setEnabled(false);

	restoreGeometry(settings.get_geometry());

	connect(timer, SIGNAL(timeout(void)), this, SLOT(timeout(void)));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(audioRecorder, SIGNAL(error(QString)), this, SLOT(on_error(QString)));
	connect(videoRecorder, SIGNAL(error(QString)), this, SLOT(on_error(QString)));
	connect(videoMuxing, SIGNAL(error(QString)), this, SLOT(on_error(QString)));

	trayIcon->show();
	show_message("Hi!", "EF Rocorder started.");

	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	stop_rec();
	settings.set_geometry(saveGeometry());
	delete configwindow;
	delete ui;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if ((reason == QSystemTrayIcon::Context) || (reason == QSystemTrayIcon::Trigger))
		show_menu();
}

void MainWindow::timeout()
{
	stop_rec();
	timer->stop();
}

void MainWindow::on_error(QString msg)
{
	show_message("Error Occurred!", msg, QSystemTrayIcon::MessageIcon::Critical);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
	}
	moved = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		move(event->globalPos() - dragPosition);
		event->accept();
		moved = true;
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (moved == false)
			triger();
	}
	else if (event->button() == Qt::RightButton)
	{
		show_menu();
	}
	moved = false;
}

void MainWindow::show_message(QString title, QString msg, QSystemTrayIcon::MessageIcon icon)
{
	trayIcon->showMessage(title, msg, icon);
}

void MainWindow::show_menu()
{
	if (recording_status == 0)
		myMenu.actions()[0]->setText("Status: Idle");
	else if (recording_status == 1)
		myMenu.actions()[0]->setText("Status: Perparing");
	else if (recording_status == 2)
		myMenu.actions()[0]->setText("Status: Recording");

	if (configwindow->isVisible())
	{
		myMenu.actions()[2]->setEnabled(false);
		myMenu.actions()[3]->setEnabled(false);
	}
	else
	{
		myMenu.actions()[2]->setEnabled(true);
		if (recording_status == 0)
			myMenu.actions()[3]->setEnabled(true);
		else
			myMenu.actions()[3]->setEnabled(false);
	}

	QPoint globalPos = QCursor::pos();
	QAction* selectedItem = myMenu.exec(globalPos);

	if (selectedItem == myMenu.actions()[2])
	{
		triger();
	} else if (selectedItem == myMenu.actions()[3])
	{
		open_settings();
	} else if (selectedItem == myMenu.actions()[5])
	{
		QApplication::quit();
	}
}

void MainWindow::start_rec()
{
	bool ret;
	if (recording_status == 0) {
		ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo-waiting.png"));
		this->repaint();
		recording_status = 1;

		QDateTime qdate = QDateTime::currentDateTime();
		QString date = QLocale{QLocale::English}.toString(qdate, "MMM_d");
		QString time = qdate.toString("HH_mm_ss");
		QString dir = settings.get_root_dir() + "\\" + settings.get_school() + "\\" + date + "\\";

		QStringList dir_list = dir.split("\\");
		dir = "";
		for (QStringList::const_iterator i = dir_list.constBegin(); i != dir_list.constEnd(); i++) {
			dir = dir + (*i) + "\\";
			if (access(dir.toLocal8Bit(), F_OK) == -1)
			{
				qDebug()<<dir<<" is not existing"<<endl;
				qDebug()<<"now make it"<<endl;
				int flag=mkdir(dir.toLocal8Bit());
			}
		}

		// setup timer
		timer->setInterval(QTime(0, 0, 0).secsTo(settings.get_max_duration())*1000);

		// setup video recorder
		QString vfilename = dir + "\\~video_rec_" + time + ".avi";
		videoRecorder->filename = vfilename;
		videoRecorder->should_stop = false;
		videoRecorder->preview = false;
		videoRecorder->width = 649;
		videoRecorder->height = 360;
		videoRecorder->set_device(settings.get_video());
		ret = videoRecorder->init();
		if (!ret)
		{
			videoRecorder->finish();
			ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo.png"));
			this->repaint();
			recording_status = 0;
			return;
		}

		// setup audio recorder
		QString afilename = dir + "\\~audio_rec_" + time + ".wma";
		audioRecorder->filename = afilename;
		audioRecorder->set_device(settings.get_audio());
		ret = audioRecorder->init();
		if (!ret)
		{
			videoRecorder->finish();
			audioRecorder->stop();
			audioRecorder->free();
			ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo.png"));
			this->repaint();
			recording_status = 0;
			return;
		}

		// setup muxing
		QString vfilename2 = dir + "\\~video_rec_" + time + "_2.avi";
		QString afilename2 = dir + "\\~audio_rec_" + time + "_2.wma";
		QString filename = dir + "\\REC_" + time + ".avi";
		videoMuxing->afilename = afilename;
		videoMuxing->vfilename = vfilename;
		videoMuxing->afilename2 = afilename2;
		videoMuxing->vfilename2 = vfilename2;
		videoMuxing->filename = filename;

		// start recording
		videoRecorder->start();
		audioRecorder->start();

		// start timer
		timer->start();

		ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo-activated.png"));
		this->repaint();
		recording_status = 2;
	}
}

void MainWindow::stop_rec()
{
	bool ret;
	if (recording_status == 2) {
		ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo-waiting.png"));
		this->repaint();
		recording_status = 1;

		// stop timer
		timer->stop();

		// stop recording
		videoRecorder->should_stop = true;
		audioRecorder->stop();

		// clean up video recorder
		videoRecorder->wait();
		videoRecorder->finish();

		// clean up audio recorder
		audioRecorder->free();

		// process muxing
		videoMuxing->process();

		ui->label_indicator->setPixmap(QPixmap(":/img/EF-logo.png"));
		this->repaint();
		recording_status = 0;
	}
}

void MainWindow::triger()
{
	if (recording_status == 2)
		stop_rec();
	else if (recording_status == 0)
		start_rec();
}

void MainWindow::open_settings()
{
	if (recording_status == 0)
	{
		configwindow->setModal(true);
		configwindow->show();
	}
}
