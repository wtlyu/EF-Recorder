#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>
#include <QAbstractButton>

#include "settings.h"
#include "videorecorder.h"
#include "audiorecorder.h"

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QDialog
{
    Q_OBJECT

public:
	explicit ConfigWindow(QWidget *parent = 0, AudioRecorder *audioRecorder = nullptr, VideoRecorder *videoRecorder = nullptr);
	~ConfigWindow();

private slots:
	void on_pushButton_video_refresh_clicked();

	void on_pushButton_audio_refresh_clicked();

	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

	void on_pushButton_root_dir_clicked();

	void tick(cv::Mat frame);

	void on_comboBox_video_list_activated(int index);

	void on_buttonBox_clicked(QAbstractButton *button);

private:
	void syncSettings();

	void showEvent(QShowEvent *event);

	void closeEvent(QCloseEvent *event);

	cv::Mat Mat2Output(cv::Mat cvImg);

	QImage Mat2QImage(cv::Mat &cvImg);

	Ui::ConfigWindow *ui;

	Settings settings;
	AudioRecorder *audioRecorder;
	VideoRecorder *videoRecorder;
};

#endif // CONFIGWINDOW_H
