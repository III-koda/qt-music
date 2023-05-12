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

#include "../player/iplayer.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void play_pause_button_clicked();
    void dir_prompt_button_clicked();
    void song_lists_item_clicked(QListWidgetItem* item);
    void next_song_button_clicked();
    void prev_song_button_clicked();
    void replay_button_clicked();
    void song_end_handler();
    void song_progressed_duration_updated(float progressed_duration);
    void song_slider_released();

private:
    void initialize_components();
    void change_song(size_t song_idx);
    void play_song();

    Ui::MainWindow* m_ui;
    QLabel* m_song_name_label;
    QLabel* m_artist_label;
    QListWidget* m_songs_list;
    QPushButton* m_play_pause_button;
    QPushButton* m_dir_prompt_button;
    QPushButton* m_next_song_button;
    QPushButton* m_prev_song_button;
    QPushButton* m_replay_button;
    QSlider* m_song_slider;
    QLabel* m_graphics_label;

    const QIcon PLAY_ICON;
    const QIcon PAUSE_ICON;

    size_t m_current_song_idx;
    IPlayer m_iplayer;
    QTimer* m_cursong_observing_thread;
    bool m_is_replaying;
};


class SongObserver : public QThread {
    Q_OBJECT

public:
    SongObserver(IPlayer* player) :
        m_iplayer(player),
        m_song_index(player->current_song_index()),
        QThread() {}

private:
    void run() {
        while(1) {
            // Update the song duration slider
            emit song_progressed_duration_changed(
                m_iplayer->song_progressed_duration());

            if (m_iplayer->is_song_ended()) {
                emit current_song_ended();
                break;
            }
            if (m_iplayer->current_song_index() != m_song_index) {
                break; // If the song is changed, cancel the observer.
            }
            sleep(1);
        }
    }

    IPlayer* m_iplayer;
    int m_song_index;

signals:
    void current_song_ended();
    void song_progressed_duration_changed(float progressed_duration);
};

#endif // MAINWINDOW_H
