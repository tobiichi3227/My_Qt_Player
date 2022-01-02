#include <stdint.h>
#include <Windows.h>
#include <iostream>
#include <WinUser.h>
#include <wrl.h>
#include <ShlObj.h>
#include <string>
#include <vector>
#define ssize_t SSIZE_T
#include <vlc/vlc.h>
#include <vlc/libvlc.h>
//#include "media.h"
#define dd '\n'
//HWND WorkerW{ nullptr };

using namespace std;

libvlc_instance_t* instance{ nullptr };
libvlc_media_player_t* media_player{ nullptr };
libvlc_media_list_t* media_list{ nullptr };
libvlc_media_list_player_t* media_list_player{ nullptr };
int playerstate;
bool is_player = false;

enum PlayerState {
	STATE_PLAY, 
	STATE_PAUSE
};

void get_audio_file(const std::wstring& command);
void set_volume(const int& volume);

//BOOL CALLBACK FindDesktop(HWND hwnd, LPARAM lp)
//{
//	auto p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
//	if (p == NULL) return 1;
//	WorkerW = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
//}

string w2s(const wstring& wstr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	string str(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), &str[0], str.size(), NULL, NULL);
	return str;
}

wstring GetVideoFilePath()
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

void setPlay(const string& url)
{
	instance = libvlc_new(0, NULL);
	media_list_player = libvlc_media_list_player_new(instance);
	auto media = libvlc_media_new_path(instance, url.c_str());
	media_player = libvlc_media_player_new_from_media(media);

	media_list = libvlc_media_list_new(instance);
	libvlc_media_list_add_media(media_list, media);
	cout << sizeof(media) << "\n";
	libvlc_media_release(media);

	libvlc_media_list_player_set_media_list(media_list_player, media_list);
	libvlc_media_list_player_set_media_player(media_list_player, media_player);
	libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
	libvlc_audio_set_volume(media_player, 100);
	libvlc_media_list_player_play(media_list_player);
	playerstate = STATE_PLAY;
	
}

void media_pause()
{
	if (playerstate == STATE_PLAY)
	{
		//libvlc_media_player_set_pause(media_player, 1);
		libvlc_media_list_player_pause(media_list_player);
		playerstate = STATE_PAUSE;
	}
	else if (playerstate == STATE_PAUSE)
	{
		//libvlc_media_player_set_pause(media_player, 0);
		libvlc_media_list_player_play(media_list_player);
		playerstate = STATE_PLAY;
	}
}

void media_stop()
{
	if (media_list_player != nullptr)
	{
		libvlc_media_list_player_stop(media_list_player);
		libvlc_media_list_player_release(media_list_player);
		//libvlc_media_list_release(media_list);
		if (media_list == nullptr) cout << "media list is empty\n";
		if (media_list_player == nullptr) cout << "media list player is empty\n";
		if (media_player == nullptr) cout << "media player is empty\n";
		if (instance == nullptr) cout << "instance is empty\n";
	}
}

void release_recollection()
{
	
	/*if (instance != nullptr)
	{
		libvlc_release(instance);
	}*/
	/*if (media_player != nullptr && media_list_player != nullptr)
	{
		libvlc_media_list_player_stop(media_list_player);
		libvlc_media_player_stop(media_player);

		libvlc_media_list_release(media_list);
		libvlc_media_list_player_release(media_list_player);
		libvlc_release(instance);
	}*/
}

void add_new_media(const string& url)
{
	if (instance != nullptr && media_list != nullptr && media_list_player != nullptr)
	{
		media_stop();
		media_list_player = libvlc_media_list_player_new(instance);
		auto media = libvlc_media_new_path(instance, url.c_str());
		media_player = libvlc_media_player_new_from_media(media);
		media_list = libvlc_media_list_new(instance);
		libvlc_media_list_add_media(media_list, media);
		libvlc_media_release(media);
		libvlc_media_list_player_set_media_list(media_list_player, media_list);
		libvlc_media_list_player_set_media_player(media_list_player, media_player);
		libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
		libvlc_audio_set_volume(media_player, 100);
		libvlc_media_list_player_play(media_list_player);
		playerstate = STATE_PLAY;
	}
}

int main()
{
	CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);

	//auto filePath = w2s(GetVideoFilePath());

	//cout << filePath << endl;

	/*auto hwnd_progman = FindWindow(L"Progman", nullptr);
	if (hwnd_progman == nullptr)
	{
		cout << "error" << "\n";
		return 1;
	}*/
	//DWORD_PTR result{ 0 };
	//SendMessageTimeout(hwnd_progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);
	//EnumWindows(FindDesktop, NULL);

	//char* videopath = new char[256];
	//WideCharToMultiByte(CP_UTF8, 0, L"C:\\dynamic.mp4", -1, videopath, 128, NULL, 0);

	//libvlc_instance_t* instance = libvlc_new(0, NULL);
	////libvlc_media_t* media = libvlc_media_new_path(instance, filePath.c_str());
	//libvlc_media_t* media = libvlc_media_new_path(instance, filePath.c_str());
	//libvlc_media_add_option(media, "--prefer-insecure");
	//libvlc_media_list_t* media_list = libvlc_media_list_new(instance);
	//libvlc_media_player_t* media_player = libvlc_media_player_new_from_media(media);
	//libvlc_media_list_player_t* media_list_player = libvlc_media_list_player_new(instance);
	
	//libvlc_media_list_add_media(media_list, media);
	//libvlc_media_release(media);

	//libvlc_media_list_player_set_media_list(media_list_player, media_list);
	//libvlc_media_list_player_set_media_player(media_list_player, media_player);
	//libvlc_media_list_player_set_playback_mode(media_list_player, libvlc_playback_mode_loop);
	////libvlc_media_player_set_hwnd(media_player, WorkerW);
	//libvlc_audio_set_volume(media_player, 100);
	//libvlc_media_list_player_play(media_list_player);
	//cin.get();

	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of((L"\\/"));
	const auto exec_path = wstring(buffer).substr(0, pos);


	string syscall{}, command{}, file_path{};
	wstring url;
	int cnt{ 0 }, init{ 0 }, _pos{ 0 }, volume{ 0 };
	while (getline(cin, syscall))
	{
		if (syscall == "system call")
		{
			getline(cin, command);
			if (command == "open file")
			{
				file_path = w2s(GetVideoFilePath());
			}
			else if (command == "get file from youtube")
			{
				cout << "URL:";
				wcin >> url;
				cin.ignore();
				wstring temp = exec_path;
				temp += L"\\yt-dlp_x86.exe -x --audio-format mp3 -o temp"; temp += to_wstring(++cnt); temp += L".%(ext)s ";
				temp += url;
				wcout << temp << dd;
				get_audio_file(temp);
			}
			else if (command == "play from download file")
			{
				auto temp = exec_path;
				temp += L"\\temp" + to_wstring(++cnt) + L".webm";
				file_path = w2s(temp);
				cout << file_path << dd;
				if (!init)
				{
					setPlay(file_path);
					init = 1;
					is_player = 1;
				}
				else
				{
					add_new_media(file_path); //must release to add new media
					is_player = 1;
				}
			}
			else if (command == "inspect entire command list")
			{
				cout << "open file" << dd << "get file from youtube" << dd << "play" << dd << "play from download file" << dd << "set volume" << dd << "pause" << dd << "start" << dd << "stop"
					<< dd << "add new" << dd << "release recollection" << dd << "delete file" << dd << "clear" << dd << "exit" << dd;
			}
			else if (command == "play")
			{
				setPlay(file_path);
				init = 1;
				is_player = 1;
			}
			else if (command == "pause" || command == "start")
			{
				media_pause();
			}
			else if (command == "release recollection")
			{
				release_recollection();
				break;
			}
			else if (command == "stop")
			{
				media_stop();
				init = 0; //released
			}
			else if (command == "add new")
			{
				file_path = w2s(GetVideoFilePath());
				add_new_media(file_path);
				is_player = 1;
			}
			else if (command == "delete file")
			{
				cin >> _pos;
				cin.ignore();
				string file_name = { "temp" + to_string(_pos) + ".webm" };
				remove(file_name.c_str());
			}
			else if (command == "set volume")
			{
				cin >> volume;
				cin.ignore();
				set_volume(volume);
			}
			else if (command == "clear") system("cls");
			else if (command == "exit") break;
		}
		else
		{
			cout << "Must use system call to execute\n";
		}
		
	}
	
	return 0;
}

void get_audio_file(const std::wstring& command)
{
	//yt-dlp_x86.exe -x --audio-format mp3 -o 'temp.%(ext)s'
	//https://www.youtube.com/watch?v=D7cW3x6cDQA
	
	if (command.empty()) return;
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

void get_audio_title()
{

}

void set_volume(const int& volume)
{
	if (is_player)
		libvlc_audio_set_volume(media_player, volume);
}
