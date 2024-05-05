#include "mainwindow.hpp"

#include <math.h>
#include <unordered_set>

#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QStandardItemModel>

#include "../utils/filesys.hpp"
#include "../service/cover_art_api.hpp"
#include "const.hpp"

#include "ui_mainwindow.h"


#define DEFAULT_COVER_ART "resources/default_cover_art.jpg"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          m_ui(new Ui::MainWindow),
                                          PLAY_ICON(QIcon("../resources/play.png")),
                                          PAUSE_ICON(QIcon("../resources/pause.png")),
                                          m_is_replaying(false) {
    m_ui->setupUi(this);
    initialize_components();
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_download_song_dialog;
}

void
MainWindow::initialize_components() {
    this->setWindowIcon(QIcon("../resources/music.png"));

    QList<QLabel*> labels = m_ui->centralwidget->findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->objectName() == "song_name_label") {
            m_song_name_label = label;
            m_song_name_label->setText("");
        }
        else if (label->objectName() == "artist_label") {
            m_artist_label = label;
            m_artist_label->setText("");
        }
        else if (label->objectName() == "graphics_label") {
            m_graphics_label = label;
            m_graphics_label->setPixmap(QPixmap(DEFAULT_COVER_ART));
        }
    }

    QList<QPushButton*> push_buttons =
        m_ui->centralwidget->findChildren<QPushButton*>();
    for (QPushButton* pbutton : push_buttons) {
        if (pbutton->objectName() == "play_pause_button") {
            m_play_pause_button = pbutton;
            connect(m_play_pause_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(play_pause_button_clicked()));
        }
        else if (pbutton->objectName() == "dir_prompt_button") {
            m_dir_prompt_button = pbutton;
            connect(m_dir_prompt_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(dir_prompt_button_clicked()));
        }
        else if (pbutton->objectName() == "download_song_button"){
            m_download_song_button = pbutton;
            connect(m_download_song_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(download_song_button_clicked()));
        }
        else if (pbutton->objectName() == "next_song_button") {
            m_next_song_button = pbutton;
            connect(m_next_song_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(next_song_button_clicked()));
        }
        else if (pbutton->objectName() == "prev_song_button") {
            m_prev_song_button = pbutton;
            connect(m_prev_song_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(prev_song_button_clicked()));
        }
        else if (pbutton->objectName() == "replay_button") {
            m_replay_button = pbutton;
            connect(m_replay_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(replay_button_clicked()));
        }
    }

    m_songs_list = m_ui->centralwidget->findChild<QListWidget*>();
    connect(m_songs_list,
            SIGNAL(itemClicked(QListWidgetItem*)),
            this,
            SLOT(song_lists_item_clicked(QListWidgetItem*)));
    QScrollBar* ver_scroll_bar = m_songs_list->verticalScrollBar();
    ver_scroll_bar->setStyleSheet(CUSTOM_VERTICAL_SCROLL_BAR);
    QScrollBar* hor_scroll_bar = m_songs_list->horizontalScrollBar();
    hor_scroll_bar->setStyleSheet(CUSTOM_HORIZONTAL_SCROLL_BAR);

    m_song_slider = m_ui->centralwidget->findChild<QSlider*>();
    connect(m_song_slider,
            SIGNAL(sliderReleased()),
            this,
            SLOT(song_slider_released()));

    m_download_song_dialog = new DownloadSongDialog(this);
}

void
MainWindow::get_notified_song_downloaded(std::string dir) {
    if (dir == m_current_dir) {
        change_directory(dir);
    }
}

void
MainWindow::change_directory(std::string dir) {
    std::vector<std::string> audio_files = files_in_dir(dir);

    if (audio_files.empty()) {
        QMessageBox::critical(
        this,
        tr("IPlayer"),
        tr("Directory does not contain any audio file!"));

        return;
    }

    bool has_valid_audio = false;
    for (const std::string& filepath : audio_files) {
        if (m_iplayer.is_audio_file_valid(filepath)){
            has_valid_audio = true;
            break;
        }
    }
    if (!has_valid_audio) {
        return;
    }

    std::string curr_song_file_path = "";
    if (m_songs_list->count() > 0 || dir == m_current_dir) {
        ISongData curr_song = m_iplayer.current_song();
        curr_song_file_path = curr_song.get_song_file_path();
    }

    // Clear the songs list and iplayer
    while (m_songs_list->count() > 0) {
        m_songs_list->takeItem(0);
    }
    m_iplayer.clear_songs();

    for (const std::string& filepath : audio_files) {
        // TODO: Check if file is audio file
        ISongData song_data = m_iplayer.add_song_to_list(filepath);
        if (!song_data.is_valid_song()) {
            continue;
        }
        std::string song_display_info =
                !song_data.get_artist().empty() && !song_data.get_song_title().empty()
                        ? song_data.get_artist() + " - " + song_data.get_song_title()
                        : get_file_name(filepath, false /* remove extension */);
        m_songs_list->addItem(song_display_info.c_str());
    }
    if (m_songs_list->count() < 1) {
        QMessageBox::critical(
            this,
            tr("IPlayer"),
            tr("Directory does not contain any audio file!"));
        return;
    }

    if (!curr_song_file_path.empty()) {
        for (int i = 0; i < m_iplayer.songs_count(); i++) {
            ISongData song = m_iplayer.get_song_at_index(i);
            if (curr_song_file_path == song.get_song_file_path()) {
                m_songs_list->setCurrentRow(i);
                m_iplayer.set_current_song_index(i);
                break;
            }
        }
    }

    m_play_pause_button->setEnabled(true);
    m_next_song_button->setEnabled(true);
    m_prev_song_button->setEnabled(true);
    m_replay_button->setEnabled(true);
    m_song_slider->setEnabled(true);
    if (dir != m_current_dir) {
        change_song(0);
        m_current_dir = dir;
        m_download_song_dialog->set_download_dir(dir);
    }
}

void
MainWindow::play_pause_button_clicked() {
    if (m_iplayer.is_playing()) {
        m_play_pause_button->setIcon(PLAY_ICON);
        m_iplayer.stop_song();
    } else {
        m_play_pause_button->setIcon(PAUSE_ICON);
        m_iplayer.continue_song();
    }
}

void
MainWindow::download_song_button_clicked() {
    m_download_song_dialog->show();
}

void
MainWindow::dir_prompt_button_clicked() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Choose Directory"),
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty()) {
        return;
    }
    change_directory(dir.toStdString());
}

void
MainWindow::song_lists_item_clicked(QListWidgetItem* item) {
    change_song(m_songs_list->row(item));
}

void
MainWindow::next_song_button_clicked() {
    change_song(m_current_song_idx < m_songs_list->count() - 1
        ? m_current_song_idx + 1
        : 0);
}

void
MainWindow::prev_song_button_clicked() {
    change_song(m_current_song_idx > 0
        ? m_current_song_idx - 1
        : m_songs_list->count() - 1);
}

void
MainWindow::replay_button_clicked() {
    m_is_replaying = !m_is_replaying;

    m_replay_button->setStyleSheet(
        m_is_replaying ? "border: 2px solid blue" : "");
    m_replay_button->update();
}

void
MainWindow::change_song(size_t song_idx) {
    m_current_song_idx = song_idx;
    play_song();

    std::string img_path = get_cover_art(m_iplayer.current_song());
    if (!img_path.empty()) {
        m_graphics_label->setPixmap(QPixmap(img_path.c_str()));
    } else {
        m_graphics_label->setPixmap(QPixmap(DEFAULT_COVER_ART));
    }
}

void
MainWindow::song_end_handler() {
    if (!m_is_replaying) {
        next_song_button_clicked();
    } else {
        play_song();
    }
}

void
MainWindow::song_progressed_duration_updated(float progressed_duration) {
    m_song_slider->setValue(round(progressed_duration));
}

void
MainWindow::song_slider_released() {
    int value = m_song_slider->value();
    m_iplayer.progress_to(value);
}

void
MainWindow::play_song() {
    // Play the song
    m_iplayer.play_song(m_current_song_idx);

    SongObserver* observer = new SongObserver(&m_iplayer);
    connect(observer,
            SIGNAL(current_song_ended()),
            SLOT(song_end_handler()));
    connect(observer,
            SIGNAL(song_progressed_duration_changed(float)),
            SLOT(song_progressed_duration_updated(float)));
    connect(observer,
            SIGNAL(finished()),
            observer,
            SLOT(deleteLater()));
    observer->start();

    // Update UI
    ISongData current_song = m_iplayer.current_song();
    m_song_name_label->setText(current_song.get_song_title().c_str());
    m_artist_label->setText(current_song.get_artist().c_str());
    m_songs_list->item(m_current_song_idx)->setSelected(true);
    m_play_pause_button->setIcon(PAUSE_ICON);
    m_song_slider->setMaximum(ceil(m_iplayer.song_duration()));
    m_song_slider->setValue(0);
}
