#include "My_Qt_Player.h"

My_Qt_Player::My_Qt_Player(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//get exec path
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of((L"\\/"));
	exec_path = std::wstring(buffer).substr(0, pos);

	QTableWidgetItem* newitem = new QTableWidgetItem(QObject::tr("name"));
	ui.audio_list_table_widget->setHorizontalHeaderItem(0, newitem);

	connect(ui.volume_slider, &QSlider::valueChanged, this, [&](const int volume) {
			if (is_player)
			{
				libvlc_audio_set_volume(media_player, volume);
			}
		});
}

std::string My_Qt_Player::w2s(const std::wstring& wstr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	std::string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), &str[0], str.size(), NULL, NULL);
	return str;
}

std::wstring My_Qt_Player::GetVideoFilePath()
{
	using Microsoft::WRL::ComPtr;
	ComPtr<IFileOpenDialog> FileDialog;

	CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(FileDialog.GetAddressOf()));

	FileDialog->SetTitle(L"選擇檔案");
	COMDLG_FILTERSPEC rgSpecial[] =
	{
		{L"VIDEO", L"*.*"},
	};

	FileDialog->SetFileTypes(1, rgSpecial);
	FileDialog->Show(NULL);

	ComPtr<IShellItem> list;
	FileDialog->GetResult(&list);

	if (list.Get())
	{
		PWSTR pszFilePath{};
		list->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
		return pszFilePath;
	}
	else
		return L"";
}

void My_Qt_Player::on_OpenFile_button_clicked() //debug function
{
    //file_path = QFileDialog::getOpenFileName(nullptr, QObject::tr("audio"), qApp->applicationDirPath(), QObject::tr("audio(*.*)"));
	file_path = QString().fromStdString(w2s(GetVideoFilePath()));
}

void My_Qt_Player::on_Play_Button_clicked()
{
	auto it = file_name.constFind(ui.audio_list_table_widget->currentItem()->text());
	if (it != file_name.constEnd() && !is_player)
	{
		ui.volume_slider->setValue(50);
		file_path = QString::fromStdString(w2s(exec_path + L"\\temp" + std::to_wstring(it.value()) + L".webm"));
		instance = libvlc_new(0, NULL);
		media_list_player = libvlc_media_list_player_new(instance);
		auto media = libvlc_media_new_path(instance, QByteArray(file_path.toLocal8Bit()).constData());
		media_player = libvlc_media_player_new_from_media(media);

		media_list = libvlc_media_list_new(instance);
		libvlc_media_list_add_media(media_list, media);
		libvlc_media_release(media);

		libvlc_media_list_player_set_media_list(media_list_player, media_list);
		libvlc_media_list_player_set_media_player(media_list_player, media_player);
		libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
		libvlc_audio_set_volume(media_player, 50);
		libvlc_media_list_player_play(media_list_player);
		playerstate = STATE_PLAY;
		init = 1;
		is_player = true;
	}
	else if (it != file_name.constEnd() && is_player)
	{
		if (media_list_player != nullptr && init == 1)
		{
			ui.volume_slider->setValue(50);
			libvlc_media_list_player_stop(media_list_player);
			libvlc_media_list_player_release(media_list_player);
			file_path = QString::fromStdString(w2s(exec_path + L"\\temp" + std::to_wstring(it.value()) + L".webm"));
			media_list_player = libvlc_media_list_player_new(instance);
			auto media = libvlc_media_new_path(instance, QByteArray(file_path.toLocal8Bit()).constData());
			media_player = libvlc_media_player_new_from_media(media);

			media_list = libvlc_media_list_new(instance);
			libvlc_media_list_add_media(media_list, media);
			libvlc_media_release(media);

			libvlc_media_list_player_set_media_list(media_list_player, media_list);
			libvlc_media_list_player_set_media_player(media_list_player, media_player);
			libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
			libvlc_audio_set_volume(media_player, 50);
			libvlc_media_list_player_play(media_list_player);
			playerstate = STATE_PLAY;
			init = 1;
			is_player = true;
		}
	}
	else
	{
		QMessageBox::information(nullptr, "debug", "crash");
	}
	//qDebug() << "test\n";
	//QByteArray byte_arr = file_path.toLocal8Bit();
	/*instance = libvlc_new(0, NULL);
	media_list_player = libvlc_media_list_player_new(instance);
	auto media = libvlc_media_new_path(instance, QByteArray(file_path.toLocal8Bit()).constData());
	media_player = libvlc_media_player_new_from_media(media);

	media_list = libvlc_media_list_new(instance);
	libvlc_media_list_add_media(media_list, media);
	libvlc_media_release(media);

	libvlc_media_list_player_set_media_list(media_list_player, media_list);
	libvlc_media_list_player_set_media_player(media_list_player, media_player);
	libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
	libvlc_audio_set_volume(media_player, 50);
	libvlc_media_list_player_play(media_list_player);
	playerstate = STATE_PLAY;
	init = 1;
	is_player = true;*/
}

void My_Qt_Player::on_Pause_Button_clicked()
{
	if (playerstate == STATE_PLAY)
	{
		libvlc_media_list_player_pause(media_list_player);
		playerstate = STATE_PAUSE;
	}
	else if (playerstate == STATE_PAUSE)
	{
		libvlc_media_list_player_play(media_list_player);
		playerstate = STATE_PLAY;
	}
}

void My_Qt_Player::on_Stop_Button_clicked()
{
	if (media_list_player != nullptr && init == 1)
	{
		libvlc_media_list_player_stop(media_list_player);
		libvlc_media_list_player_release(media_list_player);
	}
}

void My_Qt_Player::on_Add_new_audio_button_clicked()
{
	if (ui.url_line_edit->text().isEmpty() || !ui.url_line_edit->text().contains("http")) return;
	//TODO : get title
	
	ui.audio_list_table_widget->insertRow(ui.audio_list_table_widget->rowCount());
	//set [rowCount() - 1][0] = title;
	//set [rowCount() - 1][1] = processing
	//file_name[ui.audio_list_table_widget->rowCount() - 1] = title_text; 
	//file_name[title_text] = ui.audio_list_table_widget->rowCount() - 1;//title to 1...100
	file_name[QString::number(ui.audio_list_table_widget->rowCount() - 1)] = ui.audio_list_table_widget->rowCount() - 1;
	//QTableWidgetItem* title_item = new QTableWidgetItem(title_text);
	QTableWidgetItem* title_item = new QTableWidgetItem(QString::number(ui.audio_list_table_widget->rowCount() - 1));
	QTableWidgetItem* processing_item = new QTableWidgetItem("processing");
	ui.audio_list_table_widget->setItem(ui.audio_list_table_widget->rowCount() - 1, 0, title_item);
	ui.audio_list_table_widget->setItem(ui.audio_list_table_widget->rowCount() - 1, 1, processing_item);
	//call yt-dlp to download audio
	//temp += L"\\yt-dlp_x86.exe -x --audio-format mp3 -o temp"; temp += std::to_wstring(ui.audio_list_table_widget->rowCount() - 1); temp += L".%(ext)s ";
	//command += url;
	//call_yt_dlp(command);
	//processing_item = new QTableWidgetItem("process finished");
	//ui.audio_list_table_widget->setItem(ui.audio_list_table_widget->rowCount() - 1, 1, processing_item);
	
	std::wstring temp = exec_path;
	temp += L"\\yt-dlp_x86.exe -x --audio-format mp3 -o temp"; temp += std::to_wstring(ui.audio_list_table_widget->rowCount() - 1);
	temp += L".%(ext)s ";
	temp += ui.url_line_edit->text().toStdWString();
	ui.url_line_edit->setText("");
	call_yt_dlp(temp);
	processing_item = new QTableWidgetItem("process finished");
	ui.audio_list_table_widget->setItem(ui.audio_list_table_widget->rowCount() - 1, 1, processing_item);
	
}

void My_Qt_Player::call_yt_dlp(const std::wstring& command)
{
	//if (command.empty()) return;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL,
		(LPWSTR)command.c_str(),
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
	))
	{
		return;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

//void My_Qt_Player::on_call_yt_dlp_debug_button_clicked()
//{
//	QString title_text{};
//	//qDebug() << exec_path << '\n';
//	auto temp = exec_path;
//	//temp += L"\\yt-dlp_x86.exe -x --audio-format mp3 -o temp"; temp += std::to_wstring(cnt); temp += L".%(ext)s ";
//	//temp += L"https://www.youtube.com/watch?v=aPK0Ij7Eilg";
//	//qDebug() << temp << '\n';
//	//call_yt_dlp(temp);
//
//	temp = exec_path;
//	temp += L"\\yt-dlp_x86.exe --get-title "; temp += L"https://www.youtube.com/watch?v=aPK0Ij7Eilg"; //temp += L" >> title.txt";
//	call_yt_dlp(temp);
//	QString title_file_path = qApp->applicationDirPath() + "/title.txt";
//	qDebug() << title_file_path << "\n";
//	QFile title(title_file_path);
//	if (!title.open(QIODevice::ReadWrite | QIODevice::Text))
//	{
//		qDebug() << title.errorString() << " " << title.error() << "\n";
//		QMessageBox::information(nullptr, "debug", "file can't open");
//	}
//	else
//	{ 
//		QByteArray title_array{ title.readAll() };
//		title_text = QString(title_array);
//		//title.resize(0);
//		title.close();
//	} 
//	qDebug() << title_text << '\n';
//}

QString My_Qt_Player::get_title()
{
	//QString title_text{};
	//{
	//	QString title_file_path = qApp->applicationDirPath() + "/title.txt";
	//	qDebug() << title_file_path << "\n";
	//	QFile title(title_file_path);
	//	if (!title.open(QIODevice::ReadWrite | QIODevice::Text))
	//	{
	//		qDebug() << title.errorString() << " " << title.error() << "\n";
	//		QMessageBox::information(nullptr, "debug", "file can't open");
	//	}
	//	else
	//	{
	//		QByteArray title_array{ title.readAll() };
	//		title_text = QString(title_array);
	//		//title.resize(0);
	//		title.close();
	//	}
	//	qDebug() << title_text << '\n';
	//}
	// call yt-dlp to get audio title
	// command += L"\\yt-dlp_x86.exe --get-title "; command += (std::wstring)url; command += L" >> title.txt";
	// call_yt_dlp(command)
	// QString title = get_title();
	// 
	return QString();
}

My_Qt_Player::~My_Qt_Player()
{
	for (const auto& x : file_name)
	{
		remove(("temp" + std::to_string(x) + ".webm").c_str());
	}
}
