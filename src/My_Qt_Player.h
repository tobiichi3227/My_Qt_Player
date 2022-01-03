#pragma once

#include <stdint.h>
#include <QtWidgets/QMainWindow>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QBytearray>
#include <qmap.h>
//#include <QTableView> QTableWidget : QTableView
#include <QTableWidget>
#include <QSlider>
#include <qlineedit.h>
#include <stdint.h>
#include <Windows.h>
#include <iostream>
#include <WinUser.h>
#include <wrl.h>
#include <ShlObj.h>
#include <string>
#define ssize_t SSIZE_T
#include <vlc/vlc.h>
#include <vlc/libvlc.h>
#include "ui_My_Qt_Player.h"

class My_Qt_Player : public QMainWindow
{
    Q_OBJECT

public:
    My_Qt_Player(QWidget *parent = Q_NULLPTR);
    ~My_Qt_Player();
    
private:

    enum PlayerState {
        STATE_PLAY,
        STATE_PAUSE
    };

    Ui::My_Qt_PlayerClass ui;
    //std::string file_path{};
    QString file_path{};
    //QVector<QString> file_name;
    QMap<QString, qint32> file_name;
    libvlc_instance_t* instance{ nullptr };
    libvlc_media_player_t* media_player{ nullptr };
    libvlc_media_list_t* media_list{ nullptr };
    libvlc_media_list_player_t* media_list_player{ nullptr };
    std::wstring exec_path{};
    int playerstate{ 0 }, init{ 0 }, cnt{ 1 };
    bool is_player = false;

    std::string w2s(const std::wstring& wstr);
    std::wstring GetVideoFilePath();
    void call_yt_dlp(const std::wstring&);

    QString get_title();
    //void set_volume(int);

private slots:
    void on_OpenFile_button_clicked();
    void on_Play_Button_clicked();
    void on_Pause_Button_clicked();
    void on_Stop_Button_clicked();
    void on_Add_new_audio_button_clicked();
    //void on_call_yt_dlp_debug_button_clicked();

};
