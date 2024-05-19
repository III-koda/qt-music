#include "mainwindow.hpp"

#include <math.h>
#include <unordered_set>

#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QMessageBox>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QLabel>
#include <QTimer>

#include "../utils/filesys.hpp"
#include "../service/cover_art_api.hpp"
#include "const.hpp"

#include "ui_mainwindow.h"

#include "../extlib/logger.hpp"

#define DEFAULT_COVER_ART "resources/default_cover_art.jpg"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          m_ui(new Ui::MainWindow),
                                          PLAY_ICON(QIcon("../resources/play.png")),
                                          PAUSE_ICON(QIcon("../resources/pause.png")),
                                          m_songs_manager(SongsManager::get_instance()),
                                          m_is_replaying(false) {
    m_ui->setupUi(this);
    initialize_components();
    m_songs_manager->subscribe_for_song_changes(this);
}

MainWindow::~MainWindow() {
    m_songs_manager->unsubscribe_from_song_changes(this);
    delete m_ui;
    delete m_download_song_dialog;
    delete m_empty_song_dir_dialog;
    delete m_title_timer;
    delete m_single_shot_timer;
}

void
MainWindow::initialize_components() {
    Logger::get_instance()->log(LogLevel::DEBUG, "Initializing components");

    this->setWindowIcon(QIcon("../resources/music.png"));

    m_song_title_label = m_ui->song_title_label;
    m_song_title_label->setText("");

    m_graphics_label = m_ui->graphics_label;
    m_graphics_label->setPixmap(QPixmap(DEFAULT_COVER_ART));

    m_background = m_ui->background;
    m_background->setPixmap(QPixmap(DEFAULT_COVER_ART));
    QGraphicsBlurEffect* p_blur = new QGraphicsBlurEffect;
    p_blur->setBlurRadius(40);
    p_blur->setBlurHints(QGraphicsBlurEffect::QualityHint);
    m_background->setGraphicsEffect(p_blur);

    m_play_pause_button = m_ui->play_pause_button;
    connect(m_play_pause_button,
            SIGNAL(clicked()),
            this,
            SLOT(play_pause_button_clicked()));

    m_dir_prompt_button = m_ui->dir_prompt_button;
    connect(m_dir_prompt_button,
            SIGNAL(clicked()),
            this,
            SLOT(dir_prompt_button_clicked()));

    m_download_song_button = m_ui->download_song_button;
    connect(m_download_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(download_song_button_clicked()));

    m_next_song_button = m_ui->next_song_button;
    connect(m_next_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(next_song_button_clicked()));

    m_prev_song_button = m_ui->prev_song_button;
    connect(m_prev_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(prev_song_button_clicked()));

    m_replay_button = m_ui->replay_button;
    connect(m_replay_button,
            SIGNAL(clicked()),
            this,
            SLOT(replay_button_clicked()));


    m_songs_list = m_ui->songs_list;
    connect(m_songs_list,
            SIGNAL(itemClicked(QListWidgetItem*)),
            this,
            SLOT(song_lists_item_clicked(QListWidgetItem*)));
    QScrollBar* ver_scroll_bar = m_songs_list->verticalScrollBar();
    ver_scroll_bar->setStyleSheet(CUSTOM_VERTICAL_SCROLL_BAR);
    QScrollBar* hor_scroll_bar = m_songs_list->horizontalScrollBar();
    hor_scroll_bar->setStyleSheet(CUSTOM_HORIZONTAL_SCROLL_BAR);

    m_song_slider = m_ui->song_cursor_slider;
    connect(m_song_slider,
            SIGNAL(sliderReleased()),
            this,
            SLOT(song_slider_released()));

    m_title_timer = new QTimer();
    m_single_shot_timer = new QTimer();

    m_download_song_dialog = new DownloadSongDialog(this);
    m_empty_song_dir_dialog = new EmptySongDirDialog(this);
}

void
MainWindow::get_notified_song_downloaded(std::string dir) {
    Logger::get_instance()->log(LogLevel::INFO, 
                                "Song downloaded in directory: " + dir);
    if (dir == m_current_dir) {
        change_directory(dir);
    }
}

void
MainWindow::change_directory(std::string dir) {
    std::vector<std::string> audio_files = files_in_dir(dir);

    if (audio_files.empty()) {
        m_empty_song_dir_dialog->show();
    }

    std::string curr_song_file_path = "";
    if (m_songs_list->count() > 0 || dir == m_current_dir) {
        ISongData curr_song = m_songs_manager->current_song();
        curr_song_file_path = curr_song.get_song_file_path();
    }

    // Clear the songs list and iplayer
    while (m_songs_list->count() > 0) {
        m_songs_list->takeItem(0);
    }
    m_songs_manager->clear_songs();

    for (const std::string& filepath : audio_files) {
        // TODO: Check if file is audio file
        ISongData song_data = m_songs_manager->add_song_to_list(filepath);
        if (!song_data.is_valid_song()) {
            continue;
        }
        m_songs_list->addItem(song_data.generate_display_title().c_str());
    }

    if (!curr_song_file_path.empty()) {
        for (int i = 0; i < m_songs_manager->songs_count(); i++) {
            ISongData song = m_songs_manager->get_song_at_index(i);
            if (curr_song_file_path == song.get_song_file_path()) {
                m_songs_list->setCurrentRow(i);
                m_songs_manager->set_current_song_index(i);
                break;
            }
        }
    }

    bool has_songs = m_songs_manager->songs_count() != 0 && m_songs_list->count() != 0;

    m_play_pause_button->setEnabled(has_songs);
    m_next_song_button->setEnabled(has_songs);
    m_prev_song_button->setEnabled(has_songs);
    m_replay_button->setEnabled(has_songs);
    m_song_slider->setEnabled(has_songs);
    if (has_songs && dir != m_current_dir) {
        change_song(0);
        m_current_dir = dir;
    }
    Logger::get_instance()->log(LogLevel::INFO, "Changing directory: " + dir);
}

void
MainWindow::play_pause_button_clicked() {
    if (m_songs_manager->is_playing()) {
        m_play_pause_button->setIcon(PLAY_ICON);
        m_songs_manager->stop_song();
    } else {
        m_play_pause_button->setIcon(PAUSE_ICON);
        m_songs_manager->continue_song();
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

    std::string img_path = get_cover_art(m_songs_manager->current_song());
    if (!img_path.empty()) {
        m_graphics_label->setPixmap(QPixmap(img_path.c_str()));
        m_background->setPixmap(QPixmap(img_path.c_str()));
        m_background->setScaledContents(true);
    } else {
        m_graphics_label->setPixmap(QPixmap(DEFAULT_COVER_ART));
    }
}

void
MainWindow::song_slider_released() {
    int value = m_song_slider->value();
    m_songs_manager->progress_to(value);
}

void
MainWindow::play_song() {
    m_single_shot_timer->stop();
    delete m_title_timer;
    m_title_timer = new QTimer();

    // Play the song
    m_songs_manager->play_song(m_current_song_idx);

    // Update display title
    ISongData current_song = m_songs_manager->current_song();
    m_song_title_label->setText(current_song.generate_display_title().c_str());
    if (m_song_title_label->fontMetrics().width(m_song_title_label->text())
            >= m_song_title_label->width()) {
        m_song_title_label->setAlignment(Qt::AlignLeft);
        make_moving_title();
    } else {
        m_song_title_label->setAlignment(Qt::AlignCenter);
    }

    m_songs_list->item(m_current_song_idx)->setSelected(true);
    m_play_pause_button->setIcon(PAUSE_ICON);
    m_song_slider->setMaximum(ceil(m_songs_manager->song_duration()));
    m_song_slider->setValue(0);
}

void
MainWindow::make_moving_title() {
    size_t extra_space_count =
            m_song_title_label->width() / QFontMetrics(m_song_title_label->font()).width(' ');
    m_song_title_label->setText(m_song_title_label->text() +
                                std::string(extra_space_count, ' ').c_str());

    connect(m_title_timer, &QTimer::timeout, [&]() {
        QString text = m_song_title_label->text();
        text.push_back(text.front());
        text.remove(0, 1);
        m_song_title_label->setText(text);
    });

    connect(m_single_shot_timer, &QTimer::timeout, [&] {
        m_title_timer->start(200);
        delete m_single_shot_timer;
        m_single_shot_timer = new QTimer();
    });
    m_single_shot_timer->start(2000);
}

void
MainWindow::current_song_ended() {
    if (!m_is_replaying) {
        next_song_button_clicked();
    } else {
        play_song();
    }
}

void
MainWindow::song_progressed_duration_changed(float progressed_duration) {
    m_song_slider->setValue(round(progressed_duration));
}
