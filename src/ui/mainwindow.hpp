#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QThread>

#if __WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "../modules/songs_manager/songs_manager.hpp"
#include "download_song_dialog.hpp"
#include "empty_song_dir_dialog.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow, public SongListener {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void get_notified_song_downloaded(std::string dir);

    DownloadSongDialog* get_download_song_dialog() const {
        return m_download_song_dialog;
    }

    // Override from SongListener
    void current_song_ended();

    // Override from SongListener
    void song_progressed_duration_changed(float progressed_duration);

private slots:
    void play_pause_button_clicked();
    void dir_prompt_button_clicked();
    void download_song_button_clicked();
    void song_lists_item_clicked(QListWidgetItem* item);
    void next_song_button_clicked();
    void prev_song_button_clicked();
    void replay_button_clicked();
    void song_slider_released();

private:
    void initialize_components();
    void change_song(size_t song_idx);
    void play_song();
    void change_directory(std::string dir);
    void make_moving_title();

    Ui::MainWindow* m_ui;
    QLabel* m_song_title_label;
    QListWidget* m_songs_list;
    QPushButton* m_play_pause_button;
    QPushButton* m_dir_prompt_button;
    QPushButton* m_download_song_button;
    QPushButton* m_next_song_button;
    QPushButton* m_prev_song_button;
    QPushButton* m_replay_button;
    QSlider* m_song_slider;
    QLabel* m_graphics_label;
    QLabel* m_background;
    QTimer* m_title_timer; // Timer used for making moving title
    QTimer* m_single_shot_timer; // Timer used for single shot purpose

    DownloadSongDialog* m_download_song_dialog;
    EmptySongDirDialog* m_empty_song_dir_dialog;

    const QIcon PLAY_ICON;
    const QIcon PAUSE_ICON;

    size_t m_current_song_idx;
    SongsManager* m_songs_manager;
    bool m_is_replaying;
    std::string m_current_dir;
};

#endif // MAINWINDOW_H
