#include "videomuxing.h"

#include <cstdio>
#include "windows.h"

#include <QDebug>

#include "whereami.h"

#define max(a, b) (a>b)?a:b

VideoMuxing::VideoMuxing(QObject *parent) : QObject(parent)
{

}

int64_t VideoMuxing::get_file_duration(QString filename)
{
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	avformat_open_input(&pFormatCtx, filename.toLocal8Bit(), NULL, NULL);
	avformat_find_stream_info(pFormatCtx, NULL);
	int64_t duration = pFormatCtx->duration;
	// etc
	avformat_close_input(&pFormatCtx);
	avformat_free_context(pFormatCtx);

	return duration;
}

int64_t VideoMuxing::get_audio_duration()
{
	return get_file_duration(afilename);
}

int64_t VideoMuxing::get_video_duration()
{
	return get_file_duration(vfilename);
}

void ShellExecuteX(QString &cmd, QString &params, SHELLEXECUTEINFO &ShExecInfo)
{
	ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = (reinterpret_cast<const wchar_t *>(cmd.utf16()));
	ShExecInfo.lpParameters = (reinterpret_cast<const wchar_t *>(params.utf16()));
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
}

bool VideoMuxing::process()
{
	bool ret;

	// init
	qDebug() << "[videoMuxing] process";

	int64_t aduration = get_audio_duration();
	int64_t vduration = get_video_duration();

	int64_t duration = max(aduration, vduration);

	qDebug() << "[videoMuxing] audio_duration: " << aduration << " scale duration factor: " << (double)duration / aduration;
	qDebug() << "[videoMuxing] video_duration: " << vduration << " scale duration factor: " << (double)duration / vduration;

	int length = wai_getExecutablePath(NULL, 0, NULL);
	char* path = new char[length + 1];
	wai_getExecutablePath(path, length, NULL);

	char *p = strrchr(path, '\\');
	*(p + 1) = '\0';

	QString cmd_exec = QString::fromLocal8Bit(path) + "\\ffmpeg.exe";

	delete[] path;

	QString cmd_aspeedFilter =  "-i \"" + afilename + "\" -filter:a \"atempo=" + QString::number((double)duration / aduration, 10, 4) + "\" -vn \"" + afilename2 + "\"";
	QString cmd_vspeedFilter =  "-i \"" + vfilename + "\" -filter:v \"setpts=" + QString::number((double)duration / vduration, 10, 4) + "*PTS\" -b:v 10000k \"" + vfilename2 + "\"";
	QString cmd_muxing =  "-i \"" + afilename2 + "\" -i \"" + vfilename2 + "\" -c copy \"" + filename + "\"";

	SHELLEXECUTEINFO aShExecInfo = {0};
	ShellExecuteX(cmd_exec, cmd_aspeedFilter, aShExecInfo);

	SHELLEXECUTEINFO vShExecInfo = {0};
	ShellExecuteX(cmd_exec, cmd_vspeedFilter, vShExecInfo);

	WaitForSingleObject(aShExecInfo.hProcess,INFINITE);
	WaitForSingleObject(vShExecInfo.hProcess,INFINITE);

	DWORD exitcode;
	GetExitCodeProcess(aShExecInfo.hProcess, &exitcode);
	if (exitcode) {
		emit error("Video Muxing Filed: Failed to process audio file!");
		return false;
	}

	GetExitCodeProcess(vShExecInfo.hProcess, &exitcode);
	if (exitcode) {
		emit error("Video Muxing Filed: Failed to process video file!");
		return false;
	}

	SetFileAttributes((reinterpret_cast<const wchar_t *>(afilename2.utf16())), FILE_ATTRIBUTE_HIDDEN);
	SetFileAttributes((reinterpret_cast<const wchar_t *>(vfilename2.utf16())), FILE_ATTRIBUTE_HIDDEN);

	SHELLEXECUTEINFO mShExecInfo = {0};
	ShellExecuteX(cmd_exec, cmd_muxing, mShExecInfo);
	WaitForSingleObject(mShExecInfo.hProcess,INFINITE);

	GetExitCodeProcess(mShExecInfo.hProcess, &exitcode);
	if (exitcode) {
		emit error("Video Muxing Filed: Failed to process video muxing!");
		return false;
	}

	remove(afilename.toLocal8Bit());
	remove(vfilename.toLocal8Bit());
	remove(afilename2.toLocal8Bit());
	remove(vfilename2.toLocal8Bit());

//    qDebug() << cmd_aspeedFilter.toLocal8Bit();
//    qDebug() << cmd_vspeedFilter.toLocal8Bit();
//    qDebug() << cmd_muxing.toLocal8Bit();

	return true;
}
