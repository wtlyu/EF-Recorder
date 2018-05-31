#include "configwindow.h"
#include "ui_configwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QDesktopWidget>

ConfigWindow::ConfigWindow(QWidget *parent, AudioRecorder *audioRecorder, VideoRecorder *videoRecorder) :
	QDialog(parent),
	ui(new Ui::ConfigWindow)
{
	this->audioRecorder = audioRecorder;
	this->videoRecorder = videoRecorder;

	ui->setupUi(this);

	setFixedSize(this->width(), this->height());

	// regexp: optional '-' followed by between 1 and 3 digits
	QRegExp rx("^[^<>:\"/\\\\|?*]+$");
	QValidator *validator = new QRegExpValidator(rx, this);

	ui->lineEdit_school->setValidator(validator);

	qRegisterMetaType< cv::Mat >("cv::Mat");
	connect(videoRecorder, SIGNAL(tick(cv::Mat)), this, SLOT(tick(cv::Mat)));
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

cv::Mat ConfigWindow::Mat2Output(cv::Mat cvImg)
{
	if(cvImg.channels()==3)                             //3 channels color image
	{
		cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
	}
	return cvImg;
}

QImage ConfigWindow::Mat2QImage(cv::Mat & cvImg)
{
	QImage qImg;
	if(cvImg.channels()==3)                             //3 channels color image
	{

		cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
		qImg =QImage((const unsigned char*)(cvImg.data),
					cvImg.cols, cvImg.rows,
					cvImg.cols*cvImg.channels(),
					QImage::Format_RGB888);
	}
	else if(cvImg.channels()==1)                    //grayscale image
	{
		qImg =QImage((const unsigned char*)(cvImg.data),
					cvImg.cols,cvImg.rows,
					cvImg.cols*cvImg.channels(),
					QImage::Format_Indexed8);
	}
	else
	{
		qImg =QImage((const unsigned char*)(cvImg.data),
					cvImg.cols,cvImg.rows,
					cvImg.cols*cvImg.channels(),
					QImage::Format_RGB888);
	}

	return qImg;
}

void ConfigWindow::syncSettings()
{
	ui->lineEdit_root_dir->setText(settings.get_root_dir());
	ui->lineEdit_school->setText(settings.get_school());
	ui->timeEdit_max_duration->setTime(settings.get_max_duration());
	on_pushButton_video_refresh_clicked();
	on_pushButton_audio_refresh_clicked();
}

void ConfigWindow::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	syncSettings();
	this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
}

void ConfigWindow::closeEvent(QCloseEvent *event)
{
	// stop recording
	videoRecorder->should_stop = true;

	// clean up video recorder
	videoRecorder->wait();
	videoRecorder->finish();

	QWidget::closeEvent(event);
}

void ConfigWindow::on_pushButton_video_refresh_clicked()
{
	QMap<int, QString> ret = videoRecorder->get_device_list();

	ui->comboBox_video_list->clear();

	QString device_name = settings.get_video();
	int device_id = 0;

	// Print information about the devices
	for (QMap<int, QString>::const_iterator i = ret.constBegin(); i != ret.constEnd(); ++i)
	{
		ui->comboBox_video_list->insertItem(i.key(), i.value());
		if (i.value() == device_name)
			device_id = i.key() + 1;
	}

	ui->comboBox_video_list->insertItem(0, "Default");

	ui->comboBox_video_list->setCurrentIndex(device_id);
	on_comboBox_video_list_activated(device_id);
}

void ConfigWindow::on_pushButton_audio_refresh_clicked()
{
	QMap<int, QString> ret = audioRecorder->get_device_list();

	ui->comboBox_audio_list->clear();

	QString device_name = settings.get_audio();
	int device_id = 0;

	// Print information about the devices
	for (QMap<int, QString>::const_iterator i = ret.constBegin(); i != ret.constEnd(); ++i)
	{
		ui->comboBox_audio_list->insertItem(i.key(), i.value());
		if (i.value() == device_name)
			device_id = i.key();
	}

	ui->comboBox_audio_list->insertItem(0, "Default");

	ui->comboBox_audio_list->setCurrentIndex(device_id);
}

void ConfigWindow::on_buttonBox_accepted()
{
	settings.set_root_dir(ui->lineEdit_root_dir->text());
	settings.set_school(ui->lineEdit_school->text());
	settings.set_max_duration(ui->timeEdit_max_duration->time());
	settings.set_video(ui->comboBox_video_list->currentText());
	settings.set_audio(ui->comboBox_audio_list->currentText());

	close();
}

void ConfigWindow::on_buttonBox_rejected()
{
	close();
}

void ConfigWindow::on_pushButton_root_dir_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Root Directory"),
												 ui->lineEdit_root_dir->text(),
												 QFileDialog::ShowDirsOnly);

	if (dir != "")
		ui->lineEdit_root_dir->setText(dir);
}

void ConfigWindow::tick(cv::Mat frame)
{
	QImage image = Mat2QImage(frame);
	ui->label_video_preview->setPixmap(QPixmap::fromImage(image).scaled(320, 180, Qt::KeepAspectRatio));
}

void ConfigWindow::on_comboBox_video_list_activated(int index)
{
	// stop video recorder
	videoRecorder->should_stop = true;

	// clean up video recorder
	videoRecorder->wait();
	videoRecorder->finish();\

	// setup video recorder
	videoRecorder->should_stop = false;
	videoRecorder->preview = true;
	videoRecorder->set_device(ui->comboBox_video_list->currentText());
	bool ret = videoRecorder->init();
	if (!ret)
	{
		videoRecorder->finish();
		return;
	}

	// start recording
	videoRecorder->start();
}

void ConfigWindow::on_buttonBox_clicked(QAbstractButton *button)
{
	if (button==ui->buttonBox->button(QDialogButtonBox::RestoreDefaults))
	{
		settings.restore();
		syncSettings();
	}
}
